import React from "react";

import DashboardLayout from "../layouts/DashboardLayout";
import DeviceCard from "../layouts/DeviceCard.tsx";
import {COLORS} from "../colors.ts";

const Dashboard: React.FC = () =>
    (
        <DashboardLayout>
            <div className={`p-6 ${COLORS.bgPanel} rounded-2xl flex gap-6 items-start h-fit`}>
                <DeviceCard deviceId={"device001"} name={"Meneer Jansen"}/>
                <DeviceCard deviceId={"device002"} name={"Mevrouw Jansen"}/>
                <DeviceCard deviceId={"device003"} name={"Mevrouw Julsen"}/>
            </div>
        </DashboardLayout>
    );

export default Dashboard;
