import React from "react";

import DashboardLayout from "../layouts/DashboardLayout";
import DeviceCard from "../layouts/DeviceCard.tsx";

const Dashboard: React.FC = () => {
    return (
        <DashboardLayout>
            <div className="p-6 bg-gray-50 min-h-screen rounded-2xl flex gap-6">
                <DeviceCard deviceId={"device001"}/>
                <DeviceCard deviceId={"device002"}/>
                <DeviceCard deviceId={"device003"}/>
            </div>
        </DashboardLayout>
    );
};

export default Dashboard;
