import ChartLine from "../components/ChartLine .tsx";
import InBedModule from "../components/InBedModule.tsx";
import SendAlarmModule from "../components/SendAlarmModule.tsx";
import ChartHeat from "../components/ChartHeat.tsx";
import type {DataSchema} from "../hooks/fetch.ts";

type DeviceCardProps = {
    deviceId: string;
    jsonData: DataSchema[] | null;
}

const DeviceCard = (props: DeviceCardProps) => {
    return (
        <div className="flex-1">
            <h1>{props.deviceId}</h1>
            <InBedModule {...props} />
            <SendAlarmModule {...props} />
            <ChartLine {...props} />
            <ChartHeat {...props} />
        </div>
    )
}
export default DeviceCard;