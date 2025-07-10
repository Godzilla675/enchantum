const path = require('node:path');
const fs = require('node:fs');
const Database = require('better-sqlite3');

const DB_PATH = path.join(__dirname, '../../database/clinic_data.db');

// Ensure the database directory exists
fs.mkdirSync(path.dirname(DB_PATH), { recursive: true });

const db = new Database(DB_PATH);

function initDatabase() {
  db.exec(`CREATE TABLE IF NOT EXISTS checkups (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      patient_type TEXT NOT NULL,
      start_timestamp INTEGER NOT NULL,
      end_timestamp INTEGER NOT NULL,
      duration_seconds INTEGER NOT NULL
    );`);

  db.exec(`CREATE TABLE IF NOT EXISTS settings (
      key TEXT PRIMARY KEY,
      value TEXT NOT NULL
    );`);

  // Insert default setting iff table empty
  const settingsCount = db.prepare('SELECT COUNT(*) AS cnt FROM settings').get().cnt;
  if (settingsCount === 0) {
    db.prepare('INSERT INTO settings (key, value) VALUES (?, ?)').run('reminderIntervalMinutes', '5');
  }
}

// Initialize immediately
initDatabase();

function saveCheckup({ patientType, startTimestamp, endTimestamp }) {
  const durationSeconds = Math.floor((endTimestamp - startTimestamp) / 1000);
  db.prepare(
    `INSERT INTO checkups (patient_type, start_timestamp, end_timestamp, duration_seconds)
     VALUES (@patientType, @startTimestamp, @endTimestamp, @durationSeconds)`
  ).run({ patientType, startTimestamp, endTimestamp, durationSeconds });
}

function _periodBounds(period) {
  const now = Date.now();
  const d = new Date(now);
  let start;
  switch (period) {
    case 'day':
      start = new Date(d.getFullYear(), d.getMonth(), d.getDate()).getTime();
      break;
    case 'week': {
      const day = d.getDay(); // sun=0
      const diff = d.getDate() - day; // go back to Sunday
      start = new Date(d.getFullYear(), d.getMonth(), diff).getTime();
      break;
    }
    case 'month':
      start = new Date(d.getFullYear(), d.getMonth(), 1).getTime();
      break;
    default:
      throw new Error('Invalid period');
  }
  return { start, end: now };
}

function getAverageDurations(period) {
  const { start, end } = _periodBounds(period);
  return db
    .prepare(
      `SELECT patient_type, AVG(duration_seconds) AS avg_duration
       FROM checkups
       WHERE start_timestamp >= ? AND start_timestamp < ?
       GROUP BY patient_type`
    )
    .all(start, end);
}

function getSettings() {
  const rows = db.prepare('SELECT key, value FROM settings').all();
  const obj = {};
  for (const row of rows) obj[row.key] = row.value;
  return obj;
}

function updateSetting(key, value) {
  db.prepare('INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?)').run(key, String(value));
}

module.exports = {
  saveCheckup,
  getAverageDurations,
  getSettings,
  updateSetting,
};