const { app, BrowserWindow, ipcMain, Notification } = require('electron');
const path = require('path');
const db = require('./db');

// Webpack entry points are provided by electron-forge webpack plugin
// These are global constants injected during the build process

// Handle creating/removing shortcuts on Windows when installing/uninstalling.
if (require('electron-squirrel-startup')) {
  app.quit();
}

class ClinicApp {
  constructor() {
    this.miniWindow = null;
    this.mainWindow = null;
    this.activeTimersCount = 0;
    this.reminderInterval = null;
    this.reminderIntervalMinutes = 5; // Default value
  }

  async init() {
    await app.whenReady();
    
    // Initialize database
    db.initDatabase();
    
    // Load reminder interval from settings
    const settings = db.getSettings();
    this.reminderIntervalMinutes = parseInt(settings.reminderIntervalMinutes) || 5;
    
    // Setup IPC handlers
    this.setupIPC();
    
    // Start with mini window
    this.createMiniWindow();
    
    // Start reminder system
    this.startReminderSystem();
  }

  createMiniWindow() {
    // Close main window if open
    if (this.mainWindow && !this.mainWindow.isDestroyed()) {
      this.mainWindow.close();
    }

    this.miniWindow = new BrowserWindow({
      width: 600,
      height: 400,
      title: 'Clinic Stopwatch - Timer',
      webPreferences: {
        nodeIntegration: false,
        contextIsolation: true,
        preload: MINI_WINDOW_PRELOAD_WEBPACK_ENTRY,
      },
    });

    this.miniWindow.loadURL(MINI_WINDOW_WEBPACK_ENTRY);

    this.miniWindow.on('closed', () => {
      this.miniWindow = null;
    });

    // Open DevTools in development
    if (process.env.NODE_ENV === 'development') {
      this.miniWindow.webContents.openDevTools();
    }
  }

  createMainWindow() {
    // Close mini window if open
    if (this.miniWindow && !this.miniWindow.isDestroyed()) {
      this.miniWindow.close();
    }

    this.mainWindow = new BrowserWindow({
      width: 1200,
      height: 800,
      title: 'Clinic Stopwatch - Analytics',
      webPreferences: {
        nodeIntegration: false,
        contextIsolation: true,
        preload: MAIN_WINDOW_PRELOAD_WEBPACK_ENTRY,
      },
    });

    this.mainWindow.loadURL(MAIN_WINDOW_WEBPACK_ENTRY);

    this.mainWindow.on('closed', () => {
      this.mainWindow = null;
    });

    // Open DevTools in development
    if (process.env.NODE_ENV === 'development') {
      this.mainWindow.webContents.openDevTools();
    }
  }

  setupIPC() {
    // Window management
    ipcMain.handle('window:switch-to-main', async () => {
      this.createMainWindow();
    });

    ipcMain.handle('window:switch-to-mini', async () => {
      this.createMiniWindow();
    });

    // Timer operations
    ipcMain.handle('timer:save', async (event, data) => {
      return db.saveCheckup(data);
    });

    ipcMain.handle('timer:started', async () => {
      this.activeTimersCount++;
      console.log(`Active timers: ${this.activeTimersCount}`);
    });

    ipcMain.handle('timer:stopped', async () => {
      this.activeTimersCount--;
      if (this.activeTimersCount < 0) this.activeTimersCount = 0;
      console.log(`Active timers: ${this.activeTimersCount}`);
    });

    // Statistics
    ipcMain.handle('stats:get-averages', async (event, { period }) => {
      return db.getAverageDurations(period);
    });

    // Settings
    ipcMain.handle('settings:get', async () => {
      return db.getSettings();
    });

    ipcMain.handle('settings:update', async (event, { key, value }) => {
      const result = db.updateSetting(key, value);
      
      // Update reminder interval if changed
      if (key === 'reminderIntervalMinutes') {
        this.reminderIntervalMinutes = parseInt(value);
        this.restartReminderSystem();
      }
      
      return result;
    });
  }

  startReminderSystem() {
    // Clear existing interval if any
    if (this.reminderInterval) {
      clearInterval(this.reminderInterval);
    }

    // Set up new interval
    const intervalMs = this.reminderIntervalMinutes * 60 * 1000;
    this.reminderInterval = setInterval(() => {
      if (this.activeTimersCount > 0) {
        this.showNotification();
      }
    }, intervalMs);

    console.log(`Reminder system started with ${this.reminderIntervalMinutes} minute intervals`);
  }

  restartReminderSystem() {
    console.log('Restarting reminder system with new interval');
    this.startReminderSystem();
  }

  showNotification() {
    if (Notification.isSupported()) {
      const notification = new Notification({
        title: 'Clinic Reminder',
        body: 'Time is passing.',
        icon: path.join(__dirname, '../../assets/icon.png'), // Optional icon
      });

      notification.show();
    }
  }

  setupAppEvents() {
    app.on('window-all-closed', () => {
      if (process.platform !== 'darwin') {
        this.cleanup();
        app.quit();
      }
    });

    app.on('activate', () => {
      if (BrowserWindow.getAllWindows().length === 0) {
        this.createMiniWindow();
      }
    });

    app.on('before-quit', () => {
      this.cleanup();
    });
  }

  cleanup() {
    // Clear reminder interval
    if (this.reminderInterval) {
      clearInterval(this.reminderInterval);
    }
    
    // Close database connection
    db.close();
  }
}

// Initialize and start the application
const clinicApp = new ClinicApp();

// Setup app event handlers
clinicApp.setupAppEvents();

// Initialize the app
clinicApp.init().catch(console.error);