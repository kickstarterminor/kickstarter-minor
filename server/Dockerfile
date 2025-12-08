# server/Dockerfile
FROM node:20-alpine

WORKDIR /app

# use the root package.json so both frontend+backend deps are available
COPY package*.json ./
RUN npm ci

# copy the whole project in
COPY . .

# switch into server folder to run the API
WORKDIR /app/server

EXPOSE 4000
CMD ["node", "index.js"]