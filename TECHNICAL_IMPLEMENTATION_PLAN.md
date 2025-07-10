# Clinic Stopwatch App – Detailed Technical Implementation Plan

This document provides a comprehensive, step-by-step specification for constructing the **Patient Check-up Stopwatch** application.  It merges direct implementation actions with the underlying architectural rationale to guide development from empty repository to shipping, production-ready binaries.

---

## 1. Core Technology & Project Setup

| Area              | Choice | Rationale |
|-------------------|--------|-----------|
| **Framework**     | **Electron.js** | Bundles Chromium for a consistent runtime, exposes Node.js APIs for native notifications & file-system access, and ships as a single native Windows executable. |
| **Database**      | **SQLite** | Lightweight, serverless and embedded – perfect for desktop analytics.  Supports complex `AVG`, `GROUP BY` queries that would otherwise be expensive in JavaScript. |
| **DB Library**    | **better-sqlite3** | Synchronous, extremely fast, and dead-simple API – ideal for the Electron main process. |
| **Charting**      | **chart.js** | Well-documented, responsive, and animated.  Great default choice for bar charts. |
| **Build Tool**    | **Electron Forge (webpack template)** | Official, batteries-included packaging and publishing workflow. |

### Action Steps

```bash
# 1. Scaffold a new Electron project
npx create-electron-app my-clinic-app --template=webpack
cd my-clinic-app

# 2. Install runtime dependencies
npm install better-sqlite3 chart.js
```

### Directory Layout

```
my-clinic-app/
├── database/
│   └── clinic_data.db        # Created automatically on first run
├── src/
│   ├── main/
│   │   ├── index.js          # App lifecycle, window management, IPC handlers
│   │   └── db.js             # Data-Access-Layer (DAL)
│   ├── preload/
│   │   ├── main_preload.js   # Exposes secure API for the Main Window
│   │   └── mini_preload.js   # Exposes secure API for the Mini Window
│   └── renderer/
│       ├── main_window/
│       │   ├── index.html
│       │   ├── script.js
│       │   └── style.css
│       └── mini_window/
│           ├── index.html
│           ├── script.js
│           └── style.css
└── package.json
```

---

## 2. Database Schema & Data-Access Layer (`src/main/db.js`)

### Schema

| Table   | Columns |
|---------|---------|
| **checkups** | `id INTEGER PRIMARY KEY AUTOINCREMENT`  
`patient_type TEXT NOT NULL`  
`start_timestamp INTEGER NOT NULL`  
`end_timestamp   INTEGER NOT NULL`  
`duration_seconds INTEGER NOT NULL` |
| **settings** | `key TEXT PRIMARY KEY`  
`value TEXT NOT NULL` |

*`duration_seconds` is pre-computed for lightning-fast `AVG()` queries.*

### DAL — Public API

```js
saveCheckup({ patientType, startTimestamp, endTimestamp })
getAverageDurations(period)            // period ∈ {'day','week','month'}
getSettings()
updateSetting(key, value)
```

### Implementation Checklist

1. **`initDatabase()`** – executes on app start:  
   * Opens/creates `database/clinic_data.db`.  
   * Runs `CREATE TABLE IF NOT EXISTS` for both tables.  
   * Inserts default setting (`reminderIntervalMinutes=5`) if `settings` empty.
2. **Queries** – use prepared statements for speed & safety:  
   * **Insert** → `INSERT INTO checkups (...) VALUES (...);`  
   * **Averages** →
     ```sql
     SELECT patient_type,
            AVG(duration_seconds) AS avg_duration
       FROM checkups
      WHERE start_timestamp >= ? AND start_timestamp < ?
   GROUP BY patient_type;
     ```
   * **Settings** → simple `SELECT`, plus `INSERT OR REPLACE` for updates.

---

## 3. Inter-Process Communication (IPC)

Renderer processes are sandboxed – they can only reach privileged APIs through a pre-defined, secure bridge.

### Preload Layer (`src/preload/*.js`)

```js
contextBridge.exposeInMainWorld('api', {
  saveTimer:   (data)         => ipcRenderer.invoke('timer:save',      data),
  statsGetAvg: (period)       => ipcRenderer.invoke('stats:get-averages', { period }),
  settingsGet: ()             => ipcRenderer.invoke('settings:get'),
  settingsSet: (key, value)   => ipcRenderer.invoke('settings:update', { key, value }),
  timerStarted:()             => ipcRenderer.invoke('timer:started'),
  timerStopped:()             => ipcRenderer.invoke('timer:stopped'),
  switchToMain:()             => ipcRenderer.invoke('window:switch-to-main'),
  switchToMini:()             => ipcRenderer.invoke('window:switch-to-mini')
});
```

### Main-Process Handlers (`src/main/index.js`)

| Channel | Payload | Returns | Action |
|---------|---------|---------|--------|
| `window:switch-to-main` | – | `void` | Close mini, open main |
| `window:switch-to-mini` | – | `void` | Close main, open mini |
| `timer:save`            | `{patientType,startTimestamp,endTimestamp}` | `void` | `db.saveCheckup` |
| `stats:get-averages`    | `{period}` | `object[]` | `db.getAverageDurations` |
| `settings:get`          | – | `object` | `db.getSettings` |
| `settings:update`       | `{key,value}` | `void` | `db.updateSetting` |
| `timer:started`         | – | `void` | `activeTimersCount++` |
| `timer:stopped`         | – | `void` | `activeTimersCount--` |

---

## 4. Main Process Logic (`src/main/index.js`)

1. **Window Management**  
   * `createMiniWindow()` – 300×400 frameless, always-on-top.  
   * `createMainWindow()` – 900×600 resizable.  
   * Keep references (`let miniWindow`, `let mainWindow`).  
   * IPC channels orchestrate switching.

2. **Native Notifications**  
   * `let activeTimersCount = 0` tracks running timers.  
   * Fetch `reminderIntervalMinutes` on startup.  
   * `setInterval` fires every `intervalMinutes * 60 * 1000`; if `activeTimersCount > 0` show `new Notification(...)`.  
   * When settings change, clear and recreate interval.

---

## 5. Mini Window (`src/renderer/mini_window/`)

### Behaviour Flow

```
N, B, P, ... buttons ─┐
                      │      +-------------+
                      ├──►  | activeTimers |  ←───────┐
                      │      +-------------+          │
Finish Day ───────────┘                                 │  stop click
                      │                                   │
                      ▼                                   ▼
             "window:switch-to-main"           "timer:save" + move to log
```

* **State** – `const activeTimers = { [patientType]: { startTime, intervalId, element } }`.
* **Accurate Time** – always compute `Date.now() - startTime` instead of incrementing.

---

## 6. Main Window (`src/renderer/main_window/`)

1. **Analytics Panel**  
   * Three buttons feed `updateAnalytics(period)` (`'day' | 'week' | 'month'`).  
   * The bar-chart (`chart.js`) and textual list update together.

2. **Settings Panel**  
   * Fetch current settings on load.  
   * On save, send `settings:update` then show toast / notification.

---

## 7. Packaging & Distribution

```bash
npm run make          # Produces a signed Windows installer & ZIP
```

Ship the `out/make/squirrel.windows/x64/*.exe` to the clinic.

---

## 8. Future Enhancements (Backlog)

* **Data Export** – CSV/Excel for external analysis.
* **Auto Update** – Electron-Forge’s built-in Squirrel.Windows updater.
* **Cross-Platform** – macOS & Linux packaging.
* **Authentication** – simple password gate for statistics window.