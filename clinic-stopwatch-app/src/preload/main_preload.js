const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('api', {
  windowSwitchToMini: () => ipcRenderer.invoke('window:switch-to-mini'),
  statsGetAverages: (data) => ipcRenderer.invoke('stats:get-averages', data),
  settingsGet: () => ipcRenderer.invoke('settings:get'),
  settingsUpdate: (data) => ipcRenderer.invoke('settings:update', data)
});