import React, {useEffect} from "react";

import DashboardLayout from "../layouts/DashboardLayout";
import DeviceCard from "../layouts/DeviceCard.tsx";
import {useGetData} from "../hooks/fetch.ts";

const Dashboard: React.FC = () => {
    const {jsonData, refetch} = useGetData();

    useEffect(() => {
        const interval = setInterval(() => {
            refetch();
            console.log("refetch");
        }, 5000);

        return () => clearInterval(interval);
    }, [refetch]);

    return (
        <DashboardLayout>
            <div className="p-6 bg-gray-50 min-h-screen rounded-2xl flex gap-6">
                <DeviceCard deviceId={"device001"} jsonData={jsonData}/>
                <DeviceCard deviceId={"device002"} jsonData={jsonData}/>
                <DeviceCard deviceId={"device003"} jsonData={jsonData}/>
            </div>
        </DashboardLayout>
    );
};

export default Dashboard;
