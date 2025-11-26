// server/controllers/projects.controller.js
const Project = require('../models/projects.model');

function listProjects(req, res) {
    Project.getAllProjects((err, rows) => {
        if (err) {
            console.error(err);
            return res.status(500).json({ error: 'Database error' });
        }
        res.json(rows);
    });
}

function addProject(req, res) {
    const { name, goal } = req.body;

    if (!name || !goal) {
        return res.status(400).json({ error: 'name and goal are required' });
    }

    Project.createProject({ name, goal }, (err, project) => {
        if (err) {
            console.error(err);
            return res.status(500).json({ error: 'Database insert error' });
        }
        res.status(201).json(project);
    });
}

module.exports = {
    listProjects,
    addProject,
};