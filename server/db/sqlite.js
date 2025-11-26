// server/db/sqlite.js
const sqlite3 = require('sqlite3').verbose();
const path = require('path');

const dbPath = path.join(__dirname, '..', 'kickstarter.db');
const db = new sqlite3.Database(dbPath);

db.serialize(() => {
    db.run(`
    CREATE TABLE IF NOT EXISTS projects (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL,
      goal INTEGER NOT NULL,
      pledged INTEGER NOT NULL DEFAULT 0
    )
  `);

    db.get('SELECT COUNT(*) as count FROM projects', (err, row) => {
        if (err) {
            console.error('Error counting projects', err);
            return;
        }
        if (row.count === 0) {
            db.run(
                'INSERT INTO projects (name, goal, pledged) VALUES (?, ?, ?)',
                ['First Project', 1000, 250],
                (err2) => {
                    if (err2) console.error('Error seeding projects', err2);
                }
            );
        }
    });
});

module.exports = db;