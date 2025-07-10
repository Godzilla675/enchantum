const { contextBridge, ipcRenderer } = require('electron');

// Expose protected methods that allow the renderer process to use
// the ipcRenderer without exposing the entire object
contextBridge.exposeInMainWorld('api', {
  // Window management
  windowSwitchToMain: () => ipcRenderer.invoke('window:switch-to-main'),
  windowSwitchToMini: () => ipcRenderer.invoke('window:switch-to-mini'),

  // Statistics
  statsGetAverages: (data) => ipcRenderer.invoke('stats:get-averages', data),

  // Settings
  settingsGet: () => ipcRenderer.invoke('settings:get'),
  settingsUpdate: (data) => ipcRenderer.invoke('settings:update', data),
});