const { contextBridge, ipcRenderer } = require('electron');

/**
 * Expose a minimal, secure API to the renderer process.  All operations are
 * funneled through ipcRenderer.invoke so they return promises.
 */

contextBridge.exposeInMainWorld('api', {
  // Window management
  switchToMain: () => ipcRenderer.invoke('window:switch-to-main'),
  switchToMini: () => ipcRenderer.invoke('window:switch-to-mini'),

  // Timer operations
  timerSave: (data) => ipcRenderer.invoke('timer:save', data),
  timerStarted: () => ipcRenderer.invoke('timer:started'),
  timerStopped: () => ipcRenderer.invoke('timer:stopped'),

  // Analytics
  statsGetAverages: ({ period }) => ipcRenderer.invoke('stats:get-averages', { period }),

  // Settings
  settingsGet: () => ipcRenderer.invoke('settings:get'),
  settingsUpdate: ({ key, value }) => ipcRenderer.invoke('settings:update', { key, value }),
});
