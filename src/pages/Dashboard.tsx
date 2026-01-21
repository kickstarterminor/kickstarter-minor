import DashboardLayout from "../layouts/DashboardLayout";
import ReactApexChart from "react-apexcharts";

const Dashboard: React.FC = () => {
  return (
    <DashboardLayout>
      <div className="p-6 bg-gray-50 min-h-screen">
        {/* <!-- Grid Container --> */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4 auto-rows-[200px]">
          {/* <!-- 1x1 Square: Small Metric --> */}
          <div className="p-4 bg-white rounded-2xl shadow-sm border border-gray-100 flex flex-col justify-between">
            <h3 className="text-sm font-medium text-gray-500">Active Users</h3>
            <ReactApexChart
              options={{
                chart: { type: "bar" },
                xaxis: { categories: ["Jan", "Feb", "Mar"] },
              }}
              series={[{ name: "Users", data: [30, 40, 35] }]}
              type="bar"
              height="100%"
            />
          </div>

          {/* <!-- 2x1 Rectangle: Wide Timeline (Spans 2 columns) --> */}
          <div className="lg:col-span-2 p-4 bg-white rounded-2xl shadow-sm border border-gray-100">
            <h3 className="text-sm font-medium text-gray-500">
              Revenue Overview
            </h3>
            <ReactApexChart
              options={{
                chart: { type: "line" },
                xaxis: { categories: ["Jan", "Feb", "Mar", "Apr"] },
              }}
              series={[{ name: "Revenue", data: [1000, 1200, 1100, 1300] }]}
              type="line"
              height="100%"
            />
          </div>

          {/* <!-- 1x1 Square: Another Metric --> */}
          <div className="p-4 bg-white rounded-2xl shadow-sm border border-gray-100">
            <h3 className="text-sm font-medium text-gray-500">Bounce Rate</h3>
            <ReactApexChart
              options={{
                chart: { type: "pie" },
                labels: ["Bounced", "Stayed"],
              }}
              series={[45, 55]}
              type="pie"
              height="100%"
            />
          </div>

          {/* <!-- 1x2 Tall Rectangle: Vertical Data (Spans 2 rows) --> */}
          <div className="lg:row-span-2 p-4 bg-white rounded-2xl shadow-sm border border-gray-100 flex flex-col">
            <h3 className="text-sm font-medium text-gray-500">Top Locations</h3>
            <ReactApexChart
              options={{
                chart: { type: "bar" },
                plotOptions: { bar: { horizontal: true } },
                xaxis: { categories: ["USA", "UK", "Canada", "Germany"] },
              }}
              series={[{ name: "Users", data: [400, 300, 200, 100] }]}
              type="bar"
              height="100%"
            />
          </div>

          {/* <!-- 1x1 Square --> */}
          <div className="p-4 bg-white rounded-2xl shadow-sm border border-gray-100">
            <h3 className="text-sm font-medium text-gray-500">Conversions</h3>
            <ReactApexChart
              options={{
                chart: { type: "donut" },
                labels: ["Converted", "Not Converted"],
              }}
              series={[70, 30]}
              type="donut"
              height="100%"
            />
          </div>

          {/* <!-- 2x1 Wide: Large Main Chart (Spans 2 columns) --> */}
          <div className="lg:col-span-2 p-4 bg-white rounded-2xl shadow-sm border border-gray-100">
            <h3 className="text-sm font-medium text-gray-500">Market Trends</h3>
            <ReactApexChart
              options={{
                chart: { type: "area" },
                xaxis: { categories: ["Jan", "Feb", "Mar", "Apr", "May"] },
              }}
              series={[{ name: "Trends", data: [10, 20, 15, 25, 30] }]}
              type="area"
              height="100%"
            />
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default Dashboard;
