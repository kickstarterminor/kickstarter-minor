import React from 'react';
import "./DashboardLayout.css"
import leagueLogo from "../assets/lol.svg" 





interface DashboardLayoutProps {
  children: React.ReactNode;
}

const DashboardLayout: React.FC<DashboardLayoutProps> = ({ children }) => {
  return (
     <div className="flex min-h-screen w-full bg-gray-100">
      {/* Sidebar */}
      <aside className="w-64 bg-purple-200 border-r p-4 flex flex-col shrink-0">
        <div className="h-16 bg-purple-200 flex items-center justify-center mb-8 pt-4">
          <img src={leagueLogo} alt="Logo" className="h-16" />
        </div>
        <nav className="flex flex-col flex-1 space-y-2 bg-purple-100 border-purple-400 border-2 p-4 rounded-lg pb-12 m-2">
            <h2 className="text-black text-left p-2.5 font-bold">Menu</h2>
          <a href="/home" className="hover:bg-purple-200 p-2 rounded-lg text-purple-900 font-semibold transition flex items-center space-x-2">
            <span className="material-symbols-outlined text-black">home</span>
            <span>Home</span>
          </a>
          <a href="/dashboard" className="hover:bg-purple-200 p-2 rounded-lg text-purple-900 font-semibold transition flex items-center space-x-2">
            
            <span className="material-symbols-outlined text-black">dashboard</span>
            <span>Dashboard</span>
          </a>
        </nav>
      </aside>

      {/* Main content */}
      <div className="flex-1 flex-col">
        {/* Header */}
        <header className="h-32 bg-purple-200 ml-4 mb-4 p-8 flex items-center justify-center border-b border-purple-400">
          <h1 className="text-lg font-semibold">Dashboard Header</h1>
        </header>

        {/* Main content */}
        <main className="flex-1 p-6 overflow-y-auto">
          {children}
        </main>
      </div>
    </div>
  );
};

export default DashboardLayout;

