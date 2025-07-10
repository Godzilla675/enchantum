const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('api', {
  windowSwitchToMain: () => ipcRenderer.invoke('window:switch-to-main'),
  timerSave: (data) => ipcRenderer.invoke('timer:save', data),
  timerStarted: () => ipcRenderer.invoke('timer:started'),
  timerStopped: () => ipcRenderer.invoke('timer:stopped')
});