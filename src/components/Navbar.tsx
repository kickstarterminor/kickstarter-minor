import React from 'react'; 

function Navbar() { 
    return (
        <nav className="navbar">
            <div className="navbar__brand">Kickstarter Minor</div>
            <ul className="navbar__links">
                <li><a href="/">Home</a></li>
                <li><a href="/dashboard">Dashboard</a></li>
            </ul>
        </nav>
    );
}

export default Navbar