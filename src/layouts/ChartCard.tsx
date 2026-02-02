import React from "react";
import {COLORS} from "../colors.ts";

const ChartCard: React.FC<{
  children: React.ReactNode;
}> = ({ children }) => {
  return (
    <div
      className={`lg:col-span-2 p-4 ${COLORS.bgMain}  shadow-sm border ${COLORS.hover}`}
    >
      {children}
    </div>
  );
};

export default ChartCard;
