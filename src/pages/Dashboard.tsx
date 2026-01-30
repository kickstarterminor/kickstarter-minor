import React from "react";

import DashboardLayout from "../layouts/DashboardLayout";
import ChartLine from "../components/ChartLine ";
import ChartHeat from "../components/ChartHeat";
import ChartCard from "../layouts/ChartCard";

const Dashboard: React.FC = () => {
  return (
    <DashboardLayout>
      <div className="p-6 bg-gray-50 min-h-screen rounded-2xl">
        {/* <!-- Grid Container --> */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4 auto-rows-[200px]">
          {/* linechart  */}
          <ChartCard className="lg:row-span-2">
            <ChartLine />
          </ChartCard>
          {/* heatmap  */}
          <ChartCard>
            <ChartHeat />
          </ChartCard>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default Dashboard;
