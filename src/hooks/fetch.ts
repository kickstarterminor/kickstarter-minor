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

  // 2. Wrap fetch in useCallback so it's "callable" manually if needed
  const fetchData = useCallback(async () => {
    setLoading(true);
    try {
      const response = await fetch("http://145.24.237.155:8001/data");
      if (!response.ok) throw new Error("Network response was not ok");

      // sum value calculate
      const json: DataSchema[] = await response.json();

      const formattedData: number[][] = json
        .filter((item) => item.deviceId === "Device002")
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

      setData(formattedData);
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
  return { timeSumData, loading, error, refetch: fetchData };
}
