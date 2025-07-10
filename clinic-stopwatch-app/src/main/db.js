const Database = require('better-sqlite3');
const path = require('path');

let db;

function initDatabase() {
  const dbPath = path.join(__dirname, '../../database/clinic_data.db');
  db = new Database(dbPath);

  // Create tables if they don't exist
  db.exec(`
    CREATE TABLE IF NOT EXISTS checkups (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      patient_type TEXT NOT NULL,
      start_timestamp INTEGER NOT NULL,
      end_timestamp INTEGER NOT NULL,
      duration_seconds INTEGER NOT NULL
    )
  `);

  db.exec(`
    CREATE TABLE IF NOT EXISTS settings (
      key TEXT PRIMARY KEY,
      value TEXT NOT NULL
    )
  `);

  // Insert default settings if settings table is empty
  const settingsCount = db.prepare('SELECT COUNT(*) as count FROM settings').get();
  if (settingsCount.count === 0) {
    db.prepare('INSERT INTO settings (key, value) VALUES (?, ?)').run('reminderIntervalMinutes', '5');
  }
}

function saveCheckup({ patientType, startTimestamp, endTimestamp }) {
  const durationSeconds = Math.floor((endTimestamp - startTimestamp) / 1000);
  
  const stmt = db.prepare(`
    INSERT INTO checkups (patient_type, start_timestamp, end_timestamp, duration_seconds)
    VALUES (?, ?, ?, ?)
  `);
  
  stmt.run(patientType, startTimestamp, endTimestamp, durationSeconds);
}

function getAverageDurations(period) {
  const now = Date.now();
  let startTime, endTime;
  
  switch (period) {
    case 'day':
      startTime = new Date(now).setHours(0, 0, 0, 0);
      endTime = new Date(now).setHours(23, 59, 59, 999);
      break;
    case 'week':
      const weekStart = new Date(now);
      weekStart.setDate(weekStart.getDate() - weekStart.getDay());
      weekStart.setHours(0, 0, 0, 0);
      startTime = weekStart.getTime();
      endTime = startTime + (7 * 24 * 60 * 60 * 1000) - 1;
      break;
    case 'month':
      const monthStart = new Date(now);
      monthStart.setDate(1);
      monthStart.setHours(0, 0, 0, 0);
      startTime = monthStart.getTime();
      const monthEnd = new Date(now);
      monthEnd.setMonth(monthEnd.getMonth() + 1);
      monthEnd.setDate(0);
      monthEnd.setHours(23, 59, 59, 999);
      endTime = monthEnd.getTime();
      break;
    default:
      throw new Error('Invalid period specified');
  }

  const stmt = db.prepare(`
    SELECT patient_type, AVG(duration_seconds) as avg_duration
    FROM checkups
    WHERE start_timestamp >= ? AND start_timestamp < ?
    GROUP BY patient_type
  `);

  return stmt.all(startTime, endTime);
}

function getSettings() {
  const stmt = db.prepare('SELECT key, value FROM settings');
  const rows = stmt.all();
  
  const settings = {};
  rows.forEach(row => {
    settings[row.key] = row.value;
  });
  
  return settings;
}

function updateSetting(key, value) {
  const stmt = db.prepare('INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?)');
  stmt.run(key, value);
}

module.exports = {
  initDatabase,
  saveCheckup,
  getAverageDurations,
  getSettings,
  updateSetting
};