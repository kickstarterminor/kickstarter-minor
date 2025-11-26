// server/routes/projects.routes.js
const express = require('express');
const router = express.Router();
const projectsController = require('../controllers/projects.controller');

// GET /api/projects
router.get('/', projectsController.listProjects);

// POST /api/projects
router.post('/', projectsController.addProject);

module.exports = router;