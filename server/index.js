// server/index.js
const express = require('express');
const cors = require('cors');
const projectsRouter = require('./routes/projects.routes');

const app = express();
const PORT = 4000;

app.use(cors());
app.use(express.json());

// Mount all project-related endpoints at /api/projects
app.use('/api/projects', projectsRouter);

app.listen(PORT, () => {
    console.log(`API server listening on http://localhost:${PORT}`);
});