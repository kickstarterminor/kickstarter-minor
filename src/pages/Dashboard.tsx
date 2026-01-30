import type { ApexOptions } from "apexcharts";
import DashboardLayout from "../layouts/DashboardLayout";
import ReactApexChart from "react-apexcharts";
import { useGetData } from "../hooks/fetch";

const Dashboard: React.FC = () => {
  // make a get call to fetch data
  const { timeSumData, loading, error } = useGetData(); //useGetData();
  console.log("data", timeSumData);
  console.log("loading", loading);
  console.log("error", error);

  const options: ApexOptions = {
    chart: { type: "line" },
    series: [{ data: timeSumData || [] }],
    xaxis: { type: "datetime" },
  };

  return (
    <DashboardLayout>
      <div className="p-6 bg-gray-50 min-h-screen rounded-2xl">
        {/* <!-- Grid Container --> */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4 auto-rows-[200px]">
          {/* linechart  */}
          <div className="lg:col-span-4 p-4 bg-white rounded-2xl shadow-sm border border-gray-100">
            <ReactApexChart
              options={options}
              series={options.series}
              type="line"
              height="100%"
            />
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default Dashboard;
