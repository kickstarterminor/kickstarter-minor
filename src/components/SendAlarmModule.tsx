import {type JSX, useCallback, useEffect, useState} from "react";
import ChartCard from "../layouts/ChartCard.tsx";
import Button from "./Button.tsx";
import {COLORS} from "../colors.ts";

type SendAlarmProps = {
    deviceId: string;
}

const SendAlarmModule: (props: SendAlarmProps) => JSX.Element = (props: SendAlarmProps) => {
    const [status, setStatus] = useState<string>("");

    const sendAlarm = useCallback(async () => {
        try {
            const response = await fetch("http://145.24.237.155:8001/alarm", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                },
                body: JSON.stringify({
                    deviceId: props.deviceId,
                }),
            });

            if (!response.ok) {
                throw new Error("Failed to send alarm");
            }

            setStatus("Alarm sent successfully");
        } catch (err) {
            setStatus("Error sending alarm");
        }
    }, []);

    useEffect(() => {
        setTimeout(() => {
            setStatus("");
        }, 5000);
    }, [status]);

    return (
        <ChartCard>
            {status ?
                <p>{status}</p> :
                <Button
                    type="button"
                    title="Send Alarm"
                    onClick={sendAlarm}
                    className={`${COLORS.bgPanel} rounded-2xl px-4 py-2 text-white`}
                >
                    Send Alarm
                </Button>
            }
        </ChartCard>
    );
};

export default SendAlarmModule;
