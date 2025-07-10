const { app, BrowserWindow, ipcMain, Notification } = require('electron');
const path = require('node:path');

// Handle creating/removing shortcuts on Windows when installing/uninstalling.
if (require('electron-squirrel-startup')) {
  app.quit();
}

// --- Database Layer ---------------------------------------------------------
const db = require('./main/db');

// --- Globals ----------------------------------------------------------------
let mainWindow;
let miniWindow;

// Track active timers for reminder notifications
let activeTimersCount = 0;
let reminderIntervalMinutes = parseInt(db.getSettings().reminderIntervalMinutes, 10) || 5;
let reminderIntervalId;

function scheduleReminderLoop() {
  if (reminderIntervalId) clearInterval(reminderIntervalId);
  reminderIntervalId = setInterval(() => {
    if (activeTimersCount > 0) {
      new Notification({ title: 'Clinic Reminder', body: 'Time is passing.' }).show();
    }
  }, reminderIntervalMinutes * 60 * 1000);
}

// --- Window Creation --------------------------------------------------------
function createMainWindow() {
  if (mainWindow) return mainWindow;
  mainWindow = new BrowserWindow({
    width: 900,
    height: 600,
    webPreferences: {
      preload: MAIN_WINDOW_PRELOAD_WEBPACK_ENTRY,
    },
  });
  mainWindow.loadURL(MAIN_WINDOW_WEBPACK_ENTRY);
  mainWindow.on('closed', () => {
    mainWindow = null;
  });
  return mainWindow;
}

function createMiniWindow() {
  if (miniWindow) return miniWindow;
  miniWindow = new BrowserWindow({
    width: 320,
    height: 500,
    alwaysOnTop: true,
    frame: true,
    webPreferences: {
      preload: MAIN_WINDOW_PRELOAD_WEBPACK_ENTRY, // reuse same preload for now
    },
  });
  miniWindow.loadURL(MAIN_WINDOW_WEBPACK_ENTRY);
  miniWindow.on('closed', () => {
    miniWindow = null;
  });
  return miniWindow;
}

// --- IPC Handlers -----------------------------------------------------------

ipcMain.handle('window:switch-to-main', () => {
  if (miniWindow) {
    miniWindow.close();
  }
  createMainWindow();
});

ipcMain.handle('window:switch-to-mini', () => {
  if (mainWindow) {
    mainWindow.close();
  }
  createMiniWindow();
});

ipcMain.handle('timer:save', (e, data) => {
  try {
    db.saveCheckup(data);
  } catch (err) {
    console.error('Failed to save checkup', err);
  }
});

ipcMain.handle('stats:get-averages', (e, { period }) => {
  try {
    return db.getAverageDurations(period);
  } catch (err) {
    console.error('Failed to get averages', err);
    return [];
  }
});

ipcMain.handle('settings:get', () => {
  return db.getSettings();
});

ipcMain.handle('settings:update', (e, { key, value }) => {
  db.updateSetting(key, value);
  if (key === 'reminderIntervalMinutes') {
    reminderIntervalMinutes = parseInt(value, 10) || 5;
    scheduleReminderLoop();
  }
});

ipcMain.handle('timer:started', () => {
  activeTimersCount += 1;
});

ipcMain.handle('timer:stopped', () => {
  activeTimersCount = Math.max(0, activeTimersCount - 1);
});

// --- App Lifecycle ----------------------------------------------------------

app.whenReady().then(() => {
  // Start with mini window (operational interface)
  createMiniWindow();
  scheduleReminderLoop();

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createMiniWindow();
    }
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});
