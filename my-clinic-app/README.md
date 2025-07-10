# Clinic Stopwatch Application

A professional Patient Checkup Stopwatch application built with Electron.js, featuring dual-window architecture, SQLite database integration, and comprehensive analytics.

## Features

### ⏱️ Timer Interface (Mini Window)
- **8 Patient Categories**: Quick-access buttons for categories N, 1, 2, 3, 4, 5, 6, R
- **Multiple Active Timers**: Run multiple timers simultaneously for different patient categories
- **Real-time Display**: Live updating timer displays with accurate time tracking
- **Completed Sessions Log**: View completed timers for the current day
- **One-click Operation**: Start and stop timers with simple button clicks

### 📊 Analytics Dashboard (Main Window)
- **Time Period Analysis**: View data for 1 day, 1 week, or 1 month
- **Interactive Charts**: Beautiful bar charts showing average session durations
- **Statistical Summary**: Total sessions, most active category, overall averages
- **Numerical Averages**: Detailed list of average durations per category
- **Export-ready Data**: All analytics data stored in SQLite for easy export

### ⚙️ Settings & Configuration
- **Configurable Reminders**: Set reminder intervals from 1-60 minutes
- **Persistent Settings**: All settings saved to database
- **Native Notifications**: OS-level notifications when timers are active
- **Window Management**: Seamless switching between timer and analytics views

### 🔒 Security & Performance
- **Process Isolation**: Secure renderer processes with context isolation
- **SQLite Database**: Efficient local data storage with complex query support
- **Memory Management**: Proper cleanup and resource management
- **Cross-platform**: Native desktop application for Windows, macOS, and Linux

## Installation & Setup

### Prerequisites
- Node.js (version 14 or higher)
- npm or yarn package manager

### Installation Steps

1. **Clone or extract the application**
   ```bash
   cd my-clinic-app
   ```

2. **Install dependencies**
   ```bash
   npm install
   ```

3. **Start the application**
   ```bash
   npm start
   ```

   If you encounter sandbox issues on Linux:
   ```bash
   npm start -- --no-sandbox
   ```

4. **Build for production**
   ```bash
   npm run make
   ```

## Usage Guide

### Starting Timers
1. Launch the application (starts with Timer interface)
2. Click any patient category button (N, 1, 2, 3, 4, 5, 6, R)
3. Timer starts immediately and displays in the "Active Timers" section
4. Multiple timers can run simultaneously
5. Click "Stop" on any active timer to complete the session

### Viewing Analytics
1. Click "📊 View Analytics" button in the timer interface
2. Select time period: 1 Day, 1 Week, or 1 Month
3. View average durations in the left panel
4. Analyze data visually in the center chart
5. Check summary statistics in the right panel

### Configuring Settings
1. In the Analytics window, go to the Settings section (right panel)
2. Adjust reminder interval (1-60 minutes)
3. Click "Save Settings" to apply changes
4. Reminders will notify you when timers are active

### Window Management
- **Timer → Analytics**: Click "📊 View Analytics"
- **Analytics → Timer**: Click "⏱️ Back to Timer"
- Only one window is active at a time for focused workflow

## Technical Architecture

### Database Schema
```sql
-- Patient checkup sessions
CREATE TABLE checkups (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    patient_type TEXT NOT NULL,
    start_timestamp INTEGER NOT NULL,
    end_timestamp INTEGER NOT NULL,
    duration_seconds INTEGER NOT NULL
);

-- Application settings
CREATE TABLE settings (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);
```

### Project Structure
```
my-clinic-app/
├── database/
│   └── clinic_data.db          # SQLite database (auto-created)
├── src/
│   ├── main/
│   │   ├── index.js           # Main process & application logic
│   │   └── db.js              # Database access layer
│   ├── preload/
│   │   ├── main_preload.js    # Analytics window preload script
│   │   └── mini_preload.js    # Timer window preload script
│   └── renderer/
│       ├── main_window/       # Analytics dashboard
│       │   ├── index.html
│       │   ├── script.js
│       │   └── style.css
│       └── mini_window/       # Timer interface
│           ├── index.html
│           ├── script.js
│           └── style.css
└── package.json
```

### Technology Stack
- **Framework**: Electron.js (cross-platform desktop apps)
- **Database**: SQLite with better-sqlite3 (high-performance local storage)
- **Charts**: Chart.js (responsive and animated visualizations)
- **Build System**: Electron Forge with Webpack
- **IPC**: Secure context-isolated communication between processes

## Development

### Available Scripts
- `npm start` - Start development server
- `npm run package` - Package app for current platform
- `npm run make` - Build distributables for current platform
- `npm run publish` - Publish app (requires configuration)

### Development Features
- Hot reload for renderer processes
- DevTools available in development mode
- Comprehensive error handling and logging
- Memory leak prevention with proper cleanup

## Data Management

### Data Storage
- All data stored locally in SQLite database
- Database created automatically on first run
- No external dependencies or cloud services required
- Data persists between application sessions

### Data Export
The SQLite database can be accessed directly for data export:
- Location: `my-clinic-app/database/clinic_data.db`
- Use any SQLite browser or command-line tools
- Standard SQL queries supported for custom reports

### Performance Optimization
- Pre-calculated duration storage for fast analytics
- Efficient SQL queries with proper indexing
- Minimal memory footprint with lazy loading
- Real-time updates without database polling

## Troubleshooting

### Common Issues

1. **Sandbox errors on Linux**
   ```bash
   npm start -- --no-sandbox
   ```

2. **Database permission errors**
   - Ensure write permissions in application directory
   - Check that database/ folder is writable

3. **Chart.js not loading**
   - Verify internet connection for CDN
   - Check browser console for network errors

4. **Timer drift or inaccuracy**
   - Timers calculate from actual start time, not intervals
   - System sleep/hibernate may affect display updates

### Support
- Check console logs for detailed error messages
- Database issues: verify SQLite installation
- Performance issues: monitor system resources

## License

MIT License - Feel free to use and modify for your clinic's needs.

## Version History

- **v1.0.0**: Initial release with full feature set
  - Dual-window architecture
  - SQLite database integration
  - Chart.js analytics
  - Configurable notifications
  - Cross-platform support

---

**Built with ❤️ for efficient clinic management**