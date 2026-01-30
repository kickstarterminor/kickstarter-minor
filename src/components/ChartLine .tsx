import React from "react";
import type { ApexOptions } from "apexcharts";
import ReactApexChart from "react-apexcharts";
import { useGetData } from "../hooks/fetch";

const ChartLine: React.FC = () => {
  const { jsonData } = useGetData(); //useGetData();
  const twelveHoursAgo = Date.now() - 72 * 60 * 60 * 1000;

  try {
    if (!jsonData) throw new Error("No data available");
  } catch (error) {
    console.error(error);
    return null; // or some fallback UI
  }

  const formattedData: number[][] = jsonData
    .filter(
      (item) =>
        item.deviceId === "Device002" &&
        new Date(item.createdAt).getTime() > twelveHoursAgo,
    )
    .map((item) => {
      // condition to skip if by selecting device

      // Calculate the sum (skipping first value)
      const sum = item.value
        .split(",")
        .slice(1)
        .map(Number)
        .reduce((acc, curr) => acc + curr, 0);

      // Convert createdAt to a millisecond timestamp
      const timestamp = new Date(item.createdAt).getTime();

      return [timestamp, sum];
    });

  console.log("data", formattedData);

  const options: ApexOptions = {
    chart: { type: "line" },
    series: [{ data: formattedData || [] }],
    xaxis: { type: "datetime" },
  };

  return (
    <ReactApexChart
      options={options}
      series={options.series}
      type="line"
      height="100%"
    />
  );
};

export default ChartLine;
