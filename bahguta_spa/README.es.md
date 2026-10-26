# bahguta web

🇬🇧 [English](README.md) · 🇪🇸 Español

Versión web del panel bahguta Homelab: la misma información que la tablet ESP32-P4, pero en cualquier navegador, en el móvil o instalada como aplicación (PWA). Es una aplicación de una sola página en **Angular**, servida por **nginx**, que además hace de proxy hacia el backend.

> La interfaz está en español. Los textos están en las plantillas de los componentes (`src/app/**/*.html`), así que traducirla es cuestión de editarlos.

## Qué muestra

| Pantalla | Ruta | Contenido |
|---|---|---|
| Home | `/home` | Tiempo actual y una tarjeta por sección (casa, Docker, dispositivos, sistema, red, Grafana) con sus datos clave |
| Casa | `/casa` | Una tarjeta por habitación (temperatura, humedad, tiempo desde la última medida), la media de la casa y una gráfica de histórico (temperatura o humedad) |
| Tiempo | `/weather` | Tiempo actual, previsión por días con tramos horarios, fondo animado e iconos ([Meteocons](https://github.com/basmilius/weather-icons), MIT) |
| Docker | `/docker` | Los contenedores del servidor, con filtro, buscador y arrancar/parar |
| Devices | `/devices` | Dispositivos y sus sensores: estado y última medida (`/sensors` redirige aquí) |
| Red | `/red` | Clientes de la red (Omada), con buscador |
| Grafana | `/grafana` | CPU, memoria, temperatura y discos (desde Prometheus) |
| System | `/system` | Uptime, indicadores de uso, versión del backend e intervalos de refresco |

- **Una sola petición** a `GET /api/v1/dashboard/data` alimenta todas las pantallas. Se repite al intervalo que indica el backend (`apiRefreshSeconds`, 30 s por defecto) y se puede forzar con el botón de actualizar.
- **Instalable (PWA):** manifest, service worker y un botón «Instalar app» cuando el navegador lo permite.
- Tecnología: Angular 22 (componentes standalone, signals, rutas con carga diferida), TypeScript, Chart.js y SCSS.

## Arquitectura

```
Navegador ──► nginx (bahguta-web) ──► Backend Spring Boot (bahguta:8080)
                │  /                       SPA estática
                │  /api/                   proxy, solo lectura, sin clave
                └  /api/v1/system/docker/  proxy + contraseña + X-API-Key
```

La `X-API-Key` que protege el arranque y parada de contenedores **nunca está en el JavaScript**: la añade nginx, y solo en `/api/v1/system/docker/`. Esa ruta además pide usuario y contraseña (autenticación HTTP Basic).

## Desarrollo

Requisitos: Node.js 24 y npm.

```bash
cd bahguta_spa
cp proxy.conf.example.json proxy.conf.json   # pon la URL del backend y tu clave API
npm ci
npm start                                    # http://localhost:4200
```

`proxy.conf.json` está en `.gitignore`: no lo subas nunca. En desarrollo el proxy añade la clave a **todas** las peticiones `/api`, así puedes probar los botones de Docker sin nginx.

Otros comandos: `npm run build` (salida en `dist/bahguta-web/browser`) y `npm test`. Todavía no hay tests (ver el [roadmap](../docs/es/ROADMAP.md)).

## Despliegue

El `Dockerfile` compila la aplicación (Node 24) y la sirve con nginx. Antes necesitas el backend y su red Docker `web` en marcha (ver el [README principal](../README.es.md)).

```bash
# 1. Ficheros en el servidor (este ejemplo usa /opt/bahguta/web)
mkdir -p /opt/bahguta/web && cd /opt/bahguta/web
cp /ruta/al/repo/bahguta_spa/docker-compose.example.yml docker-compose.yml
cp /ruta/al/repo/bahguta_spa/.env.example .env
nano .env                       # BACKEND_URL y BAHGUTA_API_KEY (el mismo valor que el del backend)

# 2. Usuario y contraseña para el control de Docker (Basic auth de nginx)
printf 'admin:%s\n' "$(openssl passwd -apr1)" > htpasswd     # te pide la contraseña

# 3. Construir la imagen y arrancar
docker build -t bahguta-web:latest /ruta/al/repo/bahguta_spa
docker compose up -d
```

Después abre `http://TU_SERVIDOR/`. El fichero `htpasswd` debe existir **antes** de arrancar: si no, Docker crea una carpeta con ese nombre y nginx falla.

Si usas el pipeline de GitLab (`.gitlab-ci.yml`), el job de la web hace el paso 3 y la comprobación de salud en cada cambio de `bahguta_spa/`, con rollback automático; los pasos 1 y 2 se hacen a mano una sola vez.

### Configuración

| Variable | Descripción |
|---|---|
| `BACKEND_URL` | URL interna del backend, por ejemplo `http://bahguta:8080`. nginx vuelve a resolver el nombre cada pocos segundos, así que recrear el backend no obliga a reiniciar la web |
| `BAHGUTA_API_KEY` | El mismo valor que en el backend. nginx la añade solo a las peticiones de arrancar/parar |

## Seguridad

- Como el resto del proyecto, está pensada para una **red local de confianza**. **No la expongas a internet.**
- La contraseña solo protege `/api/v1/system/docker/`. El resto de `/api/` se reenvía **sin autenticación**, porque el backend no la tiene: cualquiera que llegue a la web puede leer y también llamar a los endpoints de escritura de la API (ajustes, registro de dispositivos, mediciones).
- La autenticación Basic viaja en claro por HTTP. Si necesitas acceso desde fuera, pon la web detrás de un proxy inverso con HTTPS y tu propia autenticación.
- No subas nunca `.env`, `htpasswd` ni `proxy.conf.json`.

## Estructura

```
src/app/
├── core/       servicios (consulta del dashboard, Docker), modelos y utilidades
├── layout/     shell: cabecera, menú y pie
├── pages/      una carpeta por pantalla (carga diferida)
└── shared/     componentes reutilizables (fondo animado del tiempo)
nginx/          plantilla de nginx (proxy, caché y cabeceras PWA)
public/         iconos, manifest e iconos del tiempo Meteocons
```

## Créditos

Iconos del tiempo de [Bas Milius](https://github.com/basmilius/weather-icons) con licencia MIT (`public/meteocons/LICENSE-meteocons.txt`).
