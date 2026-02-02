import React from "react";

export type ButtonProps = React.ButtonHTMLAttributes<HTMLButtonElement>;

const Button: React.FC<ButtonProps> = ({ className = "", ...props }) => {
    return (
        <button
            {...props}
            className={`w-full rounded-2xl px-4 py-2 text-white font-medium shadow-sm hover:opacity-90 transition-opacity ${className}`}
        />
    );
};

export default Button;