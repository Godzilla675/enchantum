const { app, BrowserWindow, ipcMain, Notification } = require('electron');
const path = require('path');
const db = require('./db');

let miniWindow = null;
let mainWindow = null;
let activeTimersCount = 0;
let reminderInterval = null;

function createMiniWindow() {
  miniWindow = new BrowserWindow({
    width: 400,
    height: 600,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, '../preload/mini_preload.js')
    },
    resizable: false,
    alwaysOnTop: true
  });

  miniWindow.loadFile(path.join(__dirname, '../renderer/mini_window/index.html'));

  miniWindow.on('closed', () => {
    miniWindow = null;
  });
}

function createMainWindow() {
  mainWindow = new BrowserWindow({
    width: 1200,
    height: 800,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, '../preload/main_preload.js')
    }
  });

  mainWindow.loadFile(path.join(__dirname, '../renderer/main_window/index.html'));

  mainWindow.on('closed', () => {
    mainWindow = null;
  });
}

function startReminderInterval() {
  // Clear existing interval
  if (reminderInterval) {
    clearInterval(reminderInterval);
  }

  // Get reminder interval from settings
  const settings = db.getSettings();
  const intervalMinutes = parseInt(settings.reminderIntervalMinutes) || 5;
  const intervalMs = intervalMinutes * 60 * 1000;

  reminderInterval = setInterval(() => {
    if (activeTimersCount > 0) {
      new Notification({
        title: 'Clinic Reminder',
        body: 'Time is passing.'
      }).show();
    }
  }, intervalMs);
}

// IPC Handlers
ipcMain.handle('window:switch-to-main', async () => {
  if (miniWindow) {
    miniWindow.close();
  }
  if (!mainWindow) {
    createMainWindow();
  }
});

ipcMain.handle('window:switch-to-mini', async () => {
  if (mainWindow) {
    mainWindow.close();
  }
  if (!miniWindow) {
    createMiniWindow();
  }
});

ipcMain.handle('timer:save', async (event, data) => {
  db.saveCheckup(data);
});

ipcMain.handle('timer:started', async () => {
  activeTimersCount++;
});

ipcMain.handle('timer:stopped', async () => {
  activeTimersCount = Math.max(0, activeTimersCount - 1);
});

ipcMain.handle('stats:get-averages', async (event, data) => {
  return db.getAverageDurations(data.period);
});

ipcMain.handle('settings:get', async () => {
  return db.getSettings();
});

ipcMain.handle('settings:update', async (event, data) => {
  db.updateSetting(data.key, data.value);
  
  // Restart reminder interval if reminder setting was updated
  if (data.key === 'reminderIntervalMinutes') {
    startReminderInterval();
  }
});

app.whenReady().then(() => {
  // Initialize database
  db.initDatabase();
  
  // Start reminder interval
  startReminderInterval();
  
  // Create mini window by default
  createMiniWindow();
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createMiniWindow();
  }
});

app.on('before-quit', () => {
  if (reminderInterval) {
    clearInterval(reminderInterval);
  }
});