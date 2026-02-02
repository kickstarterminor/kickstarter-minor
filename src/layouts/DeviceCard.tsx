import ChartLine from "../components/ChartLine .tsx";
import InBedModule from "../components/InBedModule.tsx";
import SendAlarmModule from "../components/SendAlarmModule.tsx";
import ChartHeat from "../components/ChartHeat.tsx";
import {type DataSchema, useGetData} from "../hooks/fetch.ts";
import {useEffect, useMemo, useState} from "react";
import MockDataModule from "../components/MockDataModule.tsx";
import TitleModule from "../components/TitleModule.tsx";

type DeviceCardProps = {
    deviceId: string;
    name: string;
}

const DeviceCard = (props: DeviceCardProps) => {
    const MOCK_DATA: DataSchema[] = useMemo(
        () => {
            const inBed = generateData(props.deviceId, 10, 8000);
            const outBed = generateData(props.deviceId, 5, 500)
            return [...inBed, ...outBed]
        },
        [props.deviceId]
    );
    const [mockData, setMockData] = useState<boolean>(false);
    const [mockIndex, setMockIndex] = useState(0);
    const {jsonData, refetch} = useGetData();

    useEffect(() => {
        const interval = setInterval(() => {
            if (mockData) {
                // advance the starting index for the mock window
                setMockIndex((prev) => {
                    const next = prev + 1;
                    return MOCK_DATA.length === 0 ? 0 : next % MOCK_DATA.length;
                });
                return;
            }

            refetch();
        }, 2000);

        return () => clearInterval(interval);
    }, [mockData, refetch, MOCK_DATA.length]);

    const deviceData = useMemo(() => {
        return (jsonData ?? []).filter((data) => {
            return data.deviceId.toLowerCase() === props.deviceId.toLowerCase();
        });
    }, [jsonData, props.deviceId]);

    const activeData = useMemo(() => {
        if (!mockData) return deviceData;
        if (MOCK_DATA.length === 0) return [];

        // rotate the array so it starts at mockIndex
        const rotated = MOCK_DATA.slice(mockIndex).concat(MOCK_DATA.slice(0, mockIndex));

        // Important: many charts sort by createdAt.
        // Re-stamp createdAt to match the rotated order so the first point moves to the back over time.
        const now = Date.now();
        const stepMs = 60_000; // 1 minute spacing
        const lastIndex = rotated.length - 1;

        return rotated.map((row, idx) => ({
            ...row,
            createdAt: new Date(now - (lastIndex - idx) * stepMs).toISOString(),
        }));
    }, [mockData, deviceData, MOCK_DATA, mockIndex]);

    return (
        <div className="w-full">
            <MockDataModule mockData={mockData} setMockData={setMockData}/>
            <TitleModule {...props}/>
            <InBedModule {...props} jsonData={activeData}/>
            <SendAlarmModule {...props}  />
            <ChartLine {...props} jsonData={activeData} slice={50}/>
            <ChartHeat {...props} jsonData={activeData}/>
        </div>
    )
}


const generateData = (deviceId: string, length: number, max: number):DataSchema[] =>
    Array.from({length}, (_, i):DataSchema => {
        const matrix = "343";
        const values = Array.from({length: 10}, () => Math.floor(Math.random() * max));
        return {
            id: `${i}`,
            value: [matrix, ...values].join(","),
            deviceId: deviceId,
            createdAt: new Date(Date.now() - i * 60000).toISOString(),
        };
    });

export default DeviceCard;