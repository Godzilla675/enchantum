const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('api', {
  // Window management
  switchToMain: () => ipcRenderer.invoke('window:switch-to-main'),
  switchToMini: () => ipcRenderer.invoke('window:switch-to-mini'),

  // Timer operations (used in mini window)
  timerSave: (data) => ipcRenderer.invoke('timer:save', data),
  timerStarted: () => ipcRenderer.invoke('timer:started'),
  timerStopped: () => ipcRenderer.invoke('timer:stopped'),

  // Analytics (not used by mini but harmless)
  statsGetAverages: ({ period }) => ipcRenderer.invoke('stats:get-averages', { period }),

  // Settings
  settingsGet: () => ipcRenderer.invoke('settings:get'),
  settingsUpdate: ({ key, value }) => ipcRenderer.invoke('settings:update', { key, value }),
});