import DashboardLayout from "../layouts/DashboardLayout";
import React from "react";

const Settings: React.FC = () => {
  return (
    <DashboardLayout>
      <div className="p-6 bg-gray-50 min-h-screen rounded-2xl">
        <h1 className="text-2xl font-semibold mb-4">Info</h1>
        {/* device info */}
        <div className="bg-white p-4 rounded-2xl shadow-sm border border-gray-100">
          <h2 className="text-xl font-semibold mb-2">Device Information</h2>
          <p className="text-gray-700 mb-1">Device ID: Device002</p>
        </div>
      </div>
    </DashboardLayout>
  );
};
export default Settings;
