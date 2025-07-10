const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('api', {
  // Window management
  windowSwitchToMini: () => ipcRenderer.invoke('window:switch-to-mini'),
  
  // Timer operations
  timerSave: (data) => ipcRenderer.invoke('timer:save', data),
  timerStarted: () => ipcRenderer.invoke('timer:started'),
  timerStopped: () => ipcRenderer.invoke('timer:stopped'),
  
  // Statistics
  statsGetAverages: (data) => ipcRenderer.invoke('stats:get-averages', data),
  
  // Settings
  settingsGet: () => ipcRenderer.invoke('settings:get'),
  settingsUpdate: (data) => ipcRenderer.invoke('settings:update', data)
});