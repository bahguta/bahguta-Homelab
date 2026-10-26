# 1) Compilar la SPA
FROM node:24-alpine AS build
WORKDIR /app
COPY package*.json ./
RUN npm ci
COPY . .
RUN npm run build -- --configuration production

# 2) Servirla con nginx (y hacer de proxy hacia el backend)
FROM nginx:alpine
# nginx sustituye ${BACKEND_URL} y ${BAHGUTA_API_KEY} al arrancar
COPY nginx/default.conf.template /etc/nginx/templates/default.conf.template
COPY --from=build /app/dist/bahguta-web/browser /usr/share/nginx/html
EXPOSE 80
