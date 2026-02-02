import {type JSX} from "react";


export type inBedProps = {
    mockData: boolean;
    setMockData: (mockData: boolean) => void;
}

const MockDataModule: (props: inBedProps) => JSX.Element = (props: inBedProps) =>
    (
            <div className="flex items-center justify-between gap-3">
                <span className="text-sm font-medium">
                    {props.mockData ? "Mocked" : "Real"}
                </span>
                <button
                    type="button"
                    role="switch"
                    aria-checked={props.mockData}
                    onClick={() => props.setMockData(!props.mockData)}
                    className={`relative inline-flex h-6 w-11 items-center rounded-full transition-colors ${props.mockData ? "bg-green-500" : "bg-gray-300"}`}
                >
                    <span
                        className={`inline-block h-4 w-4 transform rounded-full bg-white transition-transform ${props.mockData ? "translate-x-6" : "translate-x-1"}`}
                    />
                </button>
            </div>
    );

export default MockDataModule;
