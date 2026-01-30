import { useState, useEffect, useCallback } from "react";

// 1. Define what your API data looks like
interface DataSchema {
  id: string;
  value: string;
  deviceId: string;
  createdAt: string;
}

export function useGetData() {
  const [timeSumData, setData] = useState<number[][] | null>(null);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);
  const [heatData, setData2] = useState<
    { name: string; data: { x: string; y: number }[] }[] | null
  >(null);

  // 2. Wrap fetch in useCallback so it's "callable" manually if needed
  const fetchData = useCallback(async () => {
    setLoading(true);
    try {
      const response = await fetch("http://145.24.237.155:8001/data");
      if (!response.ok) throw new Error("Network response was not ok");

      // sum value calculate
      const json: DataSchema[] = await response.json();

      const twelveHoursAgo = Date.now() - 24 * 60 * 60 * 1000;

      const formattedData: number[][] = json
        .filter(
          (item) =>
            item.deviceId === "Device002" &&
            new Date(item.createdAt).getTime() > twelveHoursAgo,
        )
        .map((item) => {
          // condition to skip if by selecting device

          // Calculate the sum (skipping first value)
          const sum = item.value
            .split(",")
            .slice(1)
            .map(Number)
            .reduce((acc, curr) => acc + curr, 0);

          // Convert createdAt to a millisecond timestamp
          const timestamp = new Date(item.createdAt).getTime();

          return [timestamp, sum];
        });

      const latest = json.reduce((latest, item) => {
        return new Date(item.createdAt) > new Date(latest.createdAt)
          ? item
          : latest;
      });
      const rawvalue = latest.value;
      const numbers = rawvalue.split(",").map(Number);
      const format = numbers[0].toString().split("").map(Number);
      const value = numbers.slice(1);
      const matrix = [];

      let index = 0;

      for (const rowLength of format) {
        matrix.push(value.slice(index, index + rowLength));
        index += rowLength;
      }
      const series = matrix.map((row, rowIndex) => ({
        name: `Row ${rowIndex + 1}`,
        data: row.map((value, colIndex) => ({
          x: (colIndex + 1).toString(),
          y: value,
        })),
      }));

      setData(formattedData);
      setData2(series);
    } catch (err) {
      setError(err instanceof Error ? err.message : "Unknown error");
    } finally {
      setLoading(false);
    }
  }, []);

  // 3. Still run once on mount
  useEffect(() => {
    fetchData();
  }, [fetchData]);

  // 4. Return everything the component needs
  return { timeSumData, loading, error, refetch: fetchData, heatData };
}
