const { contextBridge, ipcRenderer } = require('electron');

// Expose protected methods that allow the renderer process to use
// the ipcRenderer without exposing the entire object
contextBridge.exposeInMainWorld('api', {
  // Window management
  windowSwitchToMain: () => ipcRenderer.invoke('window:switch-to-main'),
  windowSwitchToMini: () => ipcRenderer.invoke('window:switch-to-mini'),

  // Timer operations
  timerSave: (data) => ipcRenderer.invoke('timer:save', data),
  timerStarted: () => ipcRenderer.invoke('timer:started'),
  timerStopped: () => ipcRenderer.invoke('timer:stopped'),

  // Settings (in case needed in mini window)
  settingsGet: () => ipcRenderer.invoke('settings:get'),
  settingsUpdate: (data) => ipcRenderer.invoke('settings:update', data),
});