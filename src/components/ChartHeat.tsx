import {type JSX, useMemo} from "react";
import type {ApexOptions} from "apexcharts";
import ReactApexChart from "react-apexcharts";
import ChartCard from "../layouts/ChartCard.tsx";
import type {DataSchema} from "../hooks/fetch.ts";

const HEATMAP_COLORS = {
    // tweak these to redesign the heatmap
    ranges: [
        { from: Number.NEGATIVE_INFINITY, to: 0, color: "#E5E7EB", name: "0" },
        { from: 1, to: 100, color: "#DBEAFE", name: "1-100" },
        { from: 101, to: 500, color: "#93C5FD", name: "101-500" },
        { from: 501, to: 2000, color: "#60A5FA", name: "501-2000" },
        { from: 2001, to: 8000, color: "#1D4ED8", name: "2001-8000" },
        { from: 8001, to: Number.POSITIVE_INFINITY, color: "#0B1B4D", name: "8001+" },
    ],
};

export type ChartHeatProps = {
    jsonData: DataSchema[] | null;
}

const ChartHeat: (props: ChartHeatProps) => JSX.Element = (props: ChartHeatProps) => {
    const latest = useMemo(() => {
        if (!Array.isArray(props.jsonData) || props.jsonData.length === 0) return null;

        return props.jsonData.reduce((latestItem, item) =>
            new Date(item.createdAt) > new Date(latestItem.createdAt) ? item : latestItem,
        );
    }, [props.jsonData]);

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
            chart: { type: "heatmap" },
            series,
            legend: { show: false },
            xaxis: { type: "category" },
            plotOptions: {
                heatmap: {
                    enableShades: false,
                    colorScale: {
                        ranges: HEATMAP_COLORS.ranges,
                    },
                },
            },
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