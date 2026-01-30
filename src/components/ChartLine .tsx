import React, {useMemo} from "react";
import type {ApexOptions} from "apexcharts";
import ReactApexChart from "react-apexcharts";
import {useGetData} from "../hooks/fetch";
import ChartCard from "../layouts/ChartCard.tsx";

const ChartLine: React.FC = () => {
    const {jsonData} = useGetData(); //useGetData();

    // Keep the "time window" stable for each render cycle; recompute when the hook data changes.
    const seventyTwoHoursAgo = useMemo(
        () => Date.now() - 72 * 60 * 60 * 1000,
        [jsonData],
    );

    const formattedData: number[][] = useMemo(() => {
        if (!Array.isArray(jsonData)) return [];

        return jsonData
            .filter(
                (item) =>
                    item.deviceId === "Device002" &&
                    new Date(item.createdAt).getTime() > seventyTwoHoursAgo,
            )
            .map((item) => {
                // Calculate the sum (skipping first value)
                const sum = item.value
                    .split(",")
                    .slice(1)
                    .map(Number)
                    .reduce((acc, curr) => acc + curr, 0);

                const timestamp = new Date(item.createdAt).getTime();
                return [timestamp, sum];
            });
    }, [jsonData, seventyTwoHoursAgo]);

    const options: ApexOptions = useMemo(
        () => ({
            chart: {type: "line"},
            series: [{data: formattedData??[]}],
            xaxis: {type: "datetime"},
        }),
        [formattedData],
    );

    return (
        <ChartCard>
            <ReactApexChart
                options={options}
                series={options.series}
                type="line"
                height="100%"
            />
        </ChartCard>
    );
};

export default ChartLine;
