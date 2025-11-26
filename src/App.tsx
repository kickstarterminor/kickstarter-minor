import React, { useEffect, useState, FormEvent } from 'react';
import './App.css';

type Project = {
    id: number;
    name: string;
    goal: number;
    pledged: number;
};

function App() {
    const [projects, setProjects] = useState<Project[]>([]);
    const [loading, setLoading] = useState(true);
    const [name, setName] = useState('');
    const [goal, setGoal] = useState<number | ''>('');
    const [error, setError] = useState<string | null>(null);

    const loadProjects = async () => {
        try {
            setLoading(true);
            const res = await fetch('/api/projects');
            if (!res.ok) throw new Error('Failed to fetch projects');
            const data: Project[] = await res.json();
            setProjects(data);
        } catch (e) {
            console.error(e);
            setError('Could not load projects');
        } finally {
            setLoading(false);
        }
    };

    useEffect(() => {
        loadProjects();
    }, []);

    const handleSubmit = async (e: FormEvent) => {
        e.preventDefault();
        setError(null);

        if (!name || !goal) {
            setError('Please enter a name and goal');
            return;
        }

        try {
            const res = await fetch('/api/projects', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ name, goal: Number(goal) }),
            });

            if (!res.ok) {
                throw new Error('Failed to create project');
            }

            const newProject: Project = await res.json();
            setProjects((prev) => [...prev, newProject]);
            setName('');
            setGoal('');
        } catch (e) {
            console.error(e);
            setError('Could not create project');
        }
    };

    return (
        <div className="App">
            <h1>Kickstarter Minor</h1>

            <form onSubmit={handleSubmit}>
                <div>
                    <label>
                        Project name:{' '}
                        <input
                            value={name}
                            onChange={(e) => setName(e.target.value)}
                            required
                        />
                    </label>
                </div>
                <div>
                    <label>
                        Goal (€):{' '}
                        <input
                            type="number"
                            value={goal}
                            onChange={(e) =>
                                setGoal(e.target.value === '' ? '' : Number(e.target.value))
                            }
                            required
                            min={1}
                        />
                    </label>
                </div>
                <button type="submit">Add project</button>
            </form>

            {error && <p style={{ color: 'red' }}>{error}</p>}

            <hr />

            {loading ? (
                <p>Loading projects…</p>
            ) : projects.length === 0 ? (
                <p>No projects yet.</p>
            ) : (
                <ul>
                    {projects.map((p) => (
                        <li key={p.id}>
                            <strong>{p.name}</strong> – €{p.pledged} / €{p.goal}
                        </li>
                    ))}
                </ul>
            )}
        </div>
    );
}

export default App;