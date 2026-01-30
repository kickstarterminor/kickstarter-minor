import ChartLine from "../components/ChartLine .tsx";
import InBedModule from "../components/InBedModule.tsx";
import SendAlarmModule from "../components/SendAlarmModule.tsx";
import ChartHeat from "../components/ChartHeat.tsx";

type DeviceCardProps = {
    deviceId: string;
}

const DeviceCard = (props: DeviceCardProps) => {
    return (
        <div className="flex-1">
            <h1>{props.deviceId}</h1>
            <InBedModule/>
            <SendAlarmModule deviceId={props.deviceId}/>
            <ChartLine/>
            <ChartHeat/>
        </div>
    )
}
export default DeviceCard;