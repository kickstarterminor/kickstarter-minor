# Frontend (Vite) Dockerfile — production build + static serving

# 1) Build the Vite app
FROM node:20-alpine AS build

WORKDIR /app

# Install deps
COPY package*.json ./
RUN npm ci

# Copy source
COPY . .

# Build (Vite outputs to /app/dist by default)
RUN npm run build


# 2) Serve with nginx
FROM nginx:alpine

# SPA-friendly nginx config (fallback to index.html for client-side routing)
RUN printf 'server {\n  listen 80;\n  server_name _;\n\n  root /usr/share/nginx/html;\n  index index.html;\n\n  location / {\n    try_files $uri $uri/ /index.html;\n  }\n}\n' > /etc/nginx/conf.d/default.conf

# Copy built assets
COPY --from=build /app/dist /usr/share/nginx/html

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]