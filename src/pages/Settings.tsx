import DashboardLayout from "../layouts/DashboardLayout";
import React from "react";

const Settings: React.FC = () => {
  return (
    <DashboardLayout>
      <div className="p-6 bg-gray-50 min-h-screen rounded-2xl">
        <h1 className="text-2xl font-semibold mb-4">Settings Page</h1>
        <p>This is where settings will be configured.</p>
      </div>
    </DashboardLayout>
  );
};
export default Settings;
