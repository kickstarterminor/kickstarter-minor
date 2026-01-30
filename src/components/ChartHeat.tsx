import React from "react";
import type { ApexOptions } from "apexcharts";
import ReactApexChart from "react-apexcharts";
import { useGetData } from "../hooks/fetch";

const ChartHeat: React.FC = () => {
  const { jsonData } = useGetData();

  try {
    if (!jsonData) throw new Error("No data available");
  } catch (error) {
    console.error(error);
    return null; // or some fallback UI
  }
  const latest = jsonData.reduce((latest, item) => {
    return new Date(item.createdAt) > new Date(latest.createdAt)
      ? item
      : latest;
  });
  const rawvalue = latest.value;
  const numbers = rawvalue.split(",").map(Number);
  const format = numbers[0].toString().split("").map(Number);
  const value = numbers.slice(1);
  const matrix = [];

  let index = 0;

  for (const rowLength of format) {
    matrix.push(value.slice(index, index + rowLength));
    index += rowLength;
  }
  const series = matrix.map((row, rowIndex) => ({
    name: `Row ${rowIndex + 1}`,
    data: row.map((value, colIndex) => ({
      x: (colIndex + 1).toString(),
      y: value,
    })),
  }));

  const optionsheat: ApexOptions = {
    chart: { type: "heatmap" },
    series: series || [],
    xaxis: { type: "category" },
  };
  return (
    <ReactApexChart
      options={optionsheat}
      series={optionsheat.series}
      type="heatmap"
      height="100%"
    />
  );
};

export default ChartHeat;
