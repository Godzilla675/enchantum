const Database = require('better-sqlite3');
const path = require('path');

class ClinicDatabase {
  constructor() {
    this.db = null;
  }

  initDatabase() {
    try {
      // Create database connection
      const dbPath = path.join(__dirname, '../../database/clinic_data.db');
      this.db = new Database(dbPath);

      // Create checkups table
      this.db.exec(`
        CREATE TABLE IF NOT EXISTS checkups (
          id INTEGER PRIMARY KEY AUTOINCREMENT,
          patient_type TEXT NOT NULL,
          start_timestamp INTEGER NOT NULL,
          end_timestamp INTEGER NOT NULL,
          duration_seconds INTEGER NOT NULL
        )
      `);

      // Create settings table
      this.db.exec(`
        CREATE TABLE IF NOT EXISTS settings (
          key TEXT PRIMARY KEY,
          value TEXT NOT NULL
        )
      `);

      // Insert default settings if table is empty
      const settingsCount = this.db.prepare('SELECT COUNT(*) as count FROM settings').get();
      if (settingsCount.count === 0) {
        this.db.prepare('INSERT INTO settings (key, value) VALUES (?, ?)')
          .run('reminderIntervalMinutes', '5');
      }

      console.log('Database initialized successfully');
    } catch (error) {
      console.error('Database initialization error:', error);
    }
  }

  saveCheckup({ patientType, startTimestamp, endTimestamp }) {
    try {
      const durationSeconds = Math.floor((endTimestamp - startTimestamp) / 1000);
      
      const stmt = this.db.prepare(`
        INSERT INTO checkups (patient_type, start_timestamp, end_timestamp, duration_seconds)
        VALUES (?, ?, ?, ?)
      `);
      
      const result = stmt.run(patientType, startTimestamp, endTimestamp, durationSeconds);
      console.log(`Checkup saved with ID: ${result.lastInsertRowid}`);
      return result;
    } catch (error) {
      console.error('Error saving checkup:', error);
      throw error;
    }
  }

  getAverageDurations(period) {
    try {
      const now = Date.now();
      let startTime;

      switch (period) {
        case 'day':
          startTime = now - (24 * 60 * 60 * 1000); // 24 hours ago
          break;
        case 'week':
          startTime = now - (7 * 24 * 60 * 60 * 1000); // 7 days ago
          break;
        case 'month':
          startTime = now - (30 * 24 * 60 * 60 * 1000); // 30 days ago
          break;
        default:
          startTime = now - (24 * 60 * 60 * 1000); // Default to day
      }

      const stmt = this.db.prepare(`
        SELECT 
          patient_type, 
          AVG(duration_seconds) as avg_duration,
          COUNT(*) as count
        FROM checkups 
        WHERE start_timestamp >= ? AND start_timestamp < ?
        GROUP BY patient_type
        ORDER BY patient_type
      `);

      const results = stmt.all(startTime, now);
      return results;
    } catch (error) {
      console.error('Error getting average durations:', error);
      return [];
    }
  }

  getSettings() {
    try {
      const stmt = this.db.prepare('SELECT key, value FROM settings');
      const rows = stmt.all();
      
      const settings = {};
      rows.forEach(row => {
        settings[row.key] = row.value;
      });
      
      return settings;
    } catch (error) {
      console.error('Error getting settings:', error);
      return {};
    }
  }

  updateSetting(key, value) {
    try {
      const stmt = this.db.prepare('INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?)');
      const result = stmt.run(key, String(value));
      console.log(`Setting updated: ${key} = ${value}`);
      return result;
    } catch (error) {
      console.error('Error updating setting:', error);
      throw error;
    }
  }

  close() {
    if (this.db) {
      this.db.close();
      console.log('Database connection closed');
    }
  }
}

module.exports = new ClinicDatabase();