import React, {useMemo} from "react";
import type {ApexOptions} from "apexcharts";
import ReactApexChart from "react-apexcharts";
import {useGetData} from "../hooks/fetch";
import ChartCard from "../layouts/ChartCard.tsx";

const ChartHeat: React.FC = () => {
    const {jsonData} = useGetData();

    const latest = useMemo(() => {
        if (!Array.isArray(jsonData) || jsonData.length === 0) return null;

        return jsonData.reduce((latestItem, item) =>
            new Date(item.createdAt) > new Date(latestItem.createdAt) ? item : latestItem,
        );
    }, [jsonData]);

    const series = useMemo(() => {
        if (!latest) return [];

        const numbers = latest.value.split(",").map(Number);
        const format = numbers[0].toString().split("").map(Number);
        const value = numbers.slice(1);

        const matrix: number[][] = [];
        let index = 0;

        for (const rowLength of format) {
            matrix.push(value.slice(index, index + rowLength));
            index += rowLength;
        }

        return matrix.map((row, rowIndex) => ({
            name: `Row ${rowIndex + 1}`,
            data: row.map((cellValue, colIndex) => ({
                x: (colIndex + 1).toString(),
                y: cellValue,
            })),
        }));
    }, [latest]);

    const optionsheat: ApexOptions = useMemo(
        () => ({
            chart: {type: "heatmap"},
            series,
            xaxis: {type: "category"},
        }),
        [series],
    );

    return (
        <ChartCard>
            <ReactApexChart
                options={optionsheat}
                series={optionsheat.series}
                type="heatmap"
                height="100%"
            />
        </ChartCard>
    );
};

export default ChartHeat;