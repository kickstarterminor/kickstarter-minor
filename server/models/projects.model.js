// server/models/projects.model.js
const db = require('../db/sqlite');

function getAllProjects(callback) {
    db.all('SELECT * FROM projects', callback);
}

function createProject({ name, goal }, callback) {
    db.run(
        'INSERT INTO projects (name, goal, pledged) VALUES (?, ?, ?)',
        [name, goal, 0],
        function (err) {
            if (err) return callback(err);
            callback(null, {
                id: this.lastID,
                name,
                goal,
                pledged: 0,
            });
        }
    );
}

module.exports = {
    getAllProjects,
    createProject,
};