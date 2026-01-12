import React from 'react';

interface DashboardLayoutProps {
  children: React.ReactNode;
}

// DashboardLayout
// - Purpose: top-level layout wrapper for dashboard pages. Use this to add
//   consistent header, navigation, and page chrome around page content.
// - Props: `children` is the page content rendered inside the dashboard.
// TODOs:
// - Add a real header (logo, breadcrumbs, user menu, notifications) or
//   extract into a `Header` component once it grows.
// - Consider adding a sidebar/navigation area if your dashboard needs it.
const DashboardLayout: React.FC<DashboardLayoutProps> = ({ children }) => {
  return (
    <div className="dashboard-layout">
      <div className="dashboard-header">
        {/* Header content
            This is a placeholder for header UI (logo, page title, user avatar, etc.).
            When it becomes non-trivial, extract into a separate `Header` component
            to keep this layout simple and focused. */}
      </div>
      <div className="dashboard-content">
        {/* Page content goes here. Keep layout-related wrappers (padding, grid)
            in this container so pages only worry about their own UI. */}
        {children}
      </div>
    </div>
  );
};

export default DashboardLayout;

// Notes:
// - `React.FC` is used here mostly for convenience because it includes `children` in the type.
//   If you prefer, you can type props explicitly and use a plain arrow/function component:
//     const DashboardLayout = ({ children }: DashboardLayoutProps) => { ... }
// - Add styles for `.dashboard-layout`, `.dashboard-header`, and `.dashboard-content`
//   in your CSS files or a CSS module to match your design system.
