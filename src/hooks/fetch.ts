import { useState, useEffect, useCallback } from "react";

// 1. Define what your API data looks like
// Keep this
interface DataSchema {
  id: string;
  value: string;
  deviceId: string;
  createdAt: string;
}

export function useGetData() {
  const [jsonData, setJsonData] = useState<DataSchema[] | null>(null);
  const [loading, setLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);

  // 2. Wrap fetch in useCallback so it's "callable" manually if needed
  const fetchData = useCallback(async () => {
    setLoading(true);
    try {
      const response = await fetch("http://145.24.237.155:8001/data");
      if (!response.ok) throw new Error("Network response was not ok");

      // sum value calculate
      // Keep this
      const json: DataSchema[] = await response.json();

      setJsonData(json);
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
  return {
    loading,
    error,
    refetch: fetchData,
    jsonData,
  };
}
