import {type JSX} from "react";
import ChartCard from "../layouts/ChartCard.tsx";

type TitleProps = {
    name: string;
}

const TitleModule: (props: TitleProps) => JSX.Element = (props: TitleProps) => {
    return (
        <ChartCard>
            <h1 className="text-white text-sm md:text-base font-semibold tracking-wide">
                {props.name}
            </h1>
        </ChartCard>
    );
};

export default TitleModule;
