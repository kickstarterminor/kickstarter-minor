import React from "react";

const ChartCard: React.FC<{
  children: React.ReactNode;
  className?: string;
}> = ({ children, className = "" }) => {
  return (
    <div
      className={`lg:col-span-2 p-4 bg-white rounded-2xl shadow-sm border border-gray-100 ${className}`}
    >
      {children}
    </div>
  );
};

export default ChartCard;
