import React, {useState} from "react";
import {Link} from "react-router-dom";
import {COLORS} from "../colors.ts";

interface DashboardLayoutProps {
    children: React.ReactNode;
}

const DashboardLayout: React.FC<DashboardLayoutProps> = ({children}) => {
    const [open, setOpen] = useState(false);

    return (
        <div className={`h-screen ${COLORS.bgMain} overflow-hidden`}>
            {/* ================= MOBILE TOP BAR ================= */}
            <div className={`md:hidden flex items-center justify-between px-4 h-14 ${COLORS.bgPanel} border-b`}>
                <button
                    onClick={() => setOpen(true)}
                    className="material-symbols-outlined text-3xl"
                >
                    menu
                </button>
                <span className="font-bold">Dashboard</span>
            </div>

            <div className="flex h-[calc(100vh-3.5rem)] md:h-screen">
                {/* ================= OVERLAY ================= */}
                {open && (
                    <div
                        className="fixed inset-0 bg-black/40 z-40 md:hidden"
                        onClick={() => setOpen(false)}
                    />
                )}

                {/* ================= SIDEBAR ================= */}
                <aside
                    className={`
            fixed md:static top-0 left-0 z-50
            h-full w-64 ${COLORS.bgPanel} border-r p-4
            transform transition-transform duration-300
            ${open ? "translate-x-0" : "-translate-x-full"}
            md:translate-x-0
          `}
                >

                    {/* Navigation */}
                    <nav
                        className={`flex flex-col flex-1 space-y-2 ${COLORS.bgMain} ${COLORS.borderAccent} border-2 p-4 rounded-lg`}>
                        {/* Logo */}
                        <div className="h-16 flex items-center justify-center mb-8">
                            <img src="src/assets/Logo.png" alt="SmartSleep Logo" className="h-32 mt-8"/>
                        </div>

                        <Link
                            to="/dashboard"
                            onClick={() => setOpen(false)}
                            className={`${COLORS.hover} p-2 rounded-lg font-semibold flex items-center gap-2`}
                        >
                            <span className="material-symbols-outlined">dashboard</span>
                            Dashboard
                        </Link>

                        {/*<Link*/}
                        {/*    to="/settings"*/}
                        {/*    onClick={() => setOpen(false)}*/}
                        {/*    className={`${COLORS.hover} p-2 rounded-lg font-semibold flex items-center gap-2`}*/}
                        {/*>*/}
                        {/*    <span className="material-symbols-outlined">settings</span>*/}
                        {/*    Settings*/}
                        {/*</Link>*/}

                        <Link
                            to="/"
                            onClick={() => setOpen(false)}
                            className={`${COLORS.hover} p-2 rounded-lg font-semibold flex items-center gap-2`}
                        >
                            <span className="material-symbols-outlined">logout</span>
                            Logout
                        </Link>
                    </nav>
                </aside>

                {/* ================= MAIN CONTENT ================= */}
                <div className="flex flex-col flex-1 overflow-hidden">
                    {/* Header */}
                    {/*<header className={`hidden md:flex h-24 ${COLORS.bgPanel} border-b border-l ${COLORS.borderAccent} rounded-bl-2xl items-center justify-center`}>*/}
                    {/*  <h1 className="text-lg font-semibold">Dashboard</h1>*/}
                    {/*</header>*/}

                    {/* Page content */}
                    <main className="p-4 md:p-6 w-full">{children}</main>
                </div>
            </div>
        </div>
    );
};

export default DashboardLayout;
