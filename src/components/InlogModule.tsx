import React, { useState } from "react";

interface LoginCredentials {
  email: string;
  password: string;
}

interface jsonResponse {
  id: string;
  email: string;
  password: string;
  createdAt: string;
}

const InlogModule: React.FC = () => {
  const [credentials, setCredentials] = useState<LoginCredentials>({
    email: "",
    password: "",
  });
  const [error, setError] = useState<string>("");
  const [loading, setLoading] = useState<boolean>(false);

  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setCredentials((prev) => ({
      ...prev,
      [name]: value,
    }));
  };

  const handleSubmit = async (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();
    setError("");
    setLoading(true);

    try {
      // Simulate login API call
      const response: Response = await fetch(
        "http://145.24.237.155:8001/users",
      );
      console.log("Response status:", response.status);
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data: jsonResponse[] = await response.json();
      console.log("Data received:", data);
      const user = data.find(
        (user) =>
          user.email === credentials.email &&
          user.password === credentials.password,
      );
      console.log(user);
      if (!user) {
        setError("Ongeldige email of wachtwoord.");
        setLoading(false);
        return;
      }
      console.log("Login successful");
      // Redirect to dashboard or perform other actions upon successful login
      window.location.href = "/dashboard";

      // eslint-disable-next-line @typescript-eslint/no-unused-vars
    } catch (err) {
      setError("Login failed. Please try again.");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div
      className="flex items-center justify-center min-h-screen bg-linear-to-br from-blue-50 via-white to-indigo-100"
      id="inlog-module"
    >
      <div className="w-full max-w-md bg-white rounded-xl shadow-2xl p-8 border border-gray-200 transform transition-all duration-300 hover:scale-105">
        <div className="text-center mb-6">
          <h2 className="text-3xl font-bold text-gray-800 mb-2">Inloggen</h2>
          <p className="text-gray-600">
            Welkom terug! Log in om verder te gaan.
          </p>
        </div>
        <form onSubmit={handleSubmit} className="space-y-6">
          <div>
            <label
              htmlFor="email"
              className="block text-sm font-medium text-gray-700 mb-1"
            >
              Email
            </label>
            <div className="relative">
              <input
                type="email"
                id="email"
                name="email"
                placeholder="jouw@email.com"
                value={credentials.email}
                onChange={handleChange}
                required
                className="w-full pl-10 pr-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-transparent transition-all duration-200"
              />
              <span className="absolute left-3 top-3 text-gray-400">📧</span>
            </div>
          </div>
          <div>
            <label
              htmlFor="password"
              className="block text-sm font-medium text-gray-700 mb-1"
            >
              Wachtwoord
            </label>
            <div className="relative">
              <input
                type="password"
                id="password"
                name="password"
                placeholder="Jouw wachtwoord"
                value={credentials.password}
                onChange={handleChange}
                required
                className="w-full pl-10 pr-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-transparent transition-all duration-200"
              />
              <span className="absolute left-3 top-3 text-gray-400">🔒</span>
            </div>
          </div>

          {error && (
            <p className="text-red-500 text-sm bg-red-50 p-2 rounded">
              {error}
            </p>
          )}
          <button
            type="submit"
            disabled={loading}
            className="w-full bg-linear-to-r from-blue-500 to-indigo-600 hover:from-blue-600 hover:to-indigo-700 disabled:from-gray-400 disabled:to-gray-500 text-white font-semibold py-3 rounded-lg transition-all duration-200 transform hover:scale-105 disabled:transform-none"
          >
            {loading ? "Bezig met inloggen..." : "Inloggen"}
          </button>
        </form>
      </div>
    </div>
  );
};

export default InlogModule;
