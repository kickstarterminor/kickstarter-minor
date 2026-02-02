import {type DataSchema} from "../hooks/fetch";
import {type JSX, useMemo} from "react";
import ChartCard from "../layouts/ChartCard.tsx";
import {COLORS} from "../colors.ts";

const IN_BED_VALUE = 6000;

export type inBedProps = {
    deviceId: string;
    jsonData: DataSchema[] | null;
}

const InBedModule: (props: inBedProps) => JSX.Element = (props: inBedProps) => {
    const isInBed: boolean = useMemo(() => {
        if (!props.jsonData || props.jsonData.length ==0) {
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
            <div className="flex w-full rounded-2xl overflow-hidden border border-white/20">
                <div
                    className={`flex-1 text-center py-2 text-sm font-semibold transition-colors ${
                        isInBed ? `${COLORS.bgPanel} text-white` : "bg-transparent text-white/70"
                    }`}
                >
                    In bed
                </div>
                <div
                    className={`flex-1 text-center py-2 text-sm font-semibold transition-colors ${
                        !isInBed ? `${COLORS.bgPanel} text-white` : "bg-transparent text-white/70"
                    }`}
                >
                    Out of bed
                </div>
            </div>
        </ChartCard>
    );
};

export default InBedModule;
