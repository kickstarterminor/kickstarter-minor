import {type DataSchema} from "../hooks/fetch";
import {type JSX, useMemo} from "react";
import ChartCard from "../layouts/ChartCard.tsx";

const IN_BED_VALUE = 6000;

export type inBedProps = {
    deviceId: string;
    jsonData: DataSchema[] | null;
}

const InBedModule: (props: inBedProps) => JSX.Element = (props: inBedProps) => {
    const isInBed: boolean = useMemo(() => {
        if (!props.jsonData) {
            return false;
        }

        const lastPoint = props.jsonData[0]
        const sum = lastPoint.value
            .split(",")
            .slice(1)
            .map(Number)
            .reduce((acc, curr) => acc + curr, 0);

        return sum >= IN_BED_VALUE
    }, [props.jsonData]);

    return (
        <ChartCard>
            <button
                type="button"
                title="InBed"
                className={`${isInBed ? "bg-green-500" : "bg-red-500"} rounded-2xl px-4 py-2 text-white font-medium shadow-sm hover:opacity-90 transition-opacity`}
            >
                {isInBed ? "User is in bed" : "User is not in bed"}
            </button>
        </ChartCard>
    );
};

export default InBedModule;
