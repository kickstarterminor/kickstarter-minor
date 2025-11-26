const express = require('express');
const sqlite3 = require('sqlite3').verbose();
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = 4000;

// --- SQLite setup ---
const dbPath = path.join(__dirname, 'kickstarter.db');
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

    // Simple seed row if table is empty
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

// --- Middleware ---
app.use(cors());
app.use(express.json());

// --- Routes ---

// Get all projects
app.get('/api/projects', (req, res) => {
    db.all('SELECT * FROM projects', (err, rows) => {
        if (err) {
            console.error(err);
            return res.status(500).json({ error: 'Database error' });
        }
        res.json(rows);
    });
});

// Add a new project
app.post('/api/projects', (req, res) => {
    const { name, goal } = req.body;

    if (!name || !goal) {
        return res.status(400).json({ error: 'name and goal are required' });
    }

    db.run(
        'INSERT INTO projects (name, goal, pledged) VALUES (?, ?, ?)',
        [name, goal, 0],
        function (err) {
            if (err) {
                console.error(err);
                return res.status(500).json({ error: 'Database insert error' });
            }
            res.status(201).json({
                id: this.lastID,
                name,
                goal,
                pledged: 0,
            });
        }
    );
});

app.listen(PORT, () => {
    console.log(`API server listening on http://localhost:${PORT}`);
});