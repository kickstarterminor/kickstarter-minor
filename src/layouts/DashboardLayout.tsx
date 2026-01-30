import React from "react";
import leagueLogo from "../assets/lol.svg";
import { Link } from "react-router-dom";

interface DashboardLayoutProps {
  children: React.ReactNode;
}

const DashboardLayout: React.FC<DashboardLayoutProps> = ({ children }) => {
  return (
    <div className="flex h-screen w-full bg-blue-100">
      {/* Sidebar */}
      <aside
        className="w-64 bg-blue-200 border-r p-4 flex flex-col shrink-0"
        role="complementary"
      >
        {/* Logo */}
        <div className="h-16 bg-blue-200 flex items-center justify-center mb-8 pt-4">
          <img src={leagueLogo} alt="League of Legends Logo" className="h-16" />
        </div>
        {/* Navigation */}
        <nav className="flex flex-col flex-1 space-y-2 bg-blue-100 border-blue-400 border-2 p-4 rounded-lg pb-12 m-2">
          <h2 className="text-black text-left p-2.5 font-bold">Menu</h2>
          <ul role="list" className="flex flex-col space-y-1">
            <li>
              <Link
                to="/dashboard"
                className="hover:bg-blue-200 p-2 rounded-lg text-blue-900 font-semibold transition flex items-center space-x-2"
              >
                <span className="material-symbols-outlined text-black">
                  dashboard
                </span>
                <span>Dashboard</span>
              </Link>
            </li>
            <hr className="border-blue-400 border-t mt-2 mb-2" />
            <li className="">
              <Link
                to="/dashboard"
                className="hover:bg-blue-200 p-2 rounded-lg text-blue-900 font-semibold transition flex items-center space-x-2"
              >
                <span className="material-symbols-outlined text-black">
                  settings
                </span>
                <span>Settings</span>
              </Link>
            </li>
            <li>
              <Link
                to="/"
                className="hover:bg-blue-200 p-2 rounded-lg text-blue-900 font-semibold transition flex items-center space-x-2"
              >
                <span className="material-symbols-outlined text-black">
                  logout
                </span>
                <span>Logout</span>
              </Link>
            </li>
          </ul>
        </nav>
      </aside>

      {/* Main content */}
      <div className="flex flex-1 flex-col">
        {/* Header */}
        <header className="h-32 bg-blue-200 ml-4 mb-4 p-8 flex items-center justify-center border-b-2 border-l-2 border-blue-400 rounded-b-2xl">
          <h1 className="text-lg font-semibold">Dashboard Header</h1>
        </header>

        {/* Main content */}
        <main className="flex-1 p-6 pt-3 overflow-y-auto">{children}</main>
      </div>
    </div>
  );
};

export default DashboardLayout;
