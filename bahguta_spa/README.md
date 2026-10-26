# bahguta web

🇬🇧 English · 🇪🇸 [Español](README.es.md)

Web version of the bahguta Homelab panel: the same information as the ESP32-P4 tablet, but in any browser, on your phone or installed as an app (PWA). It is an **Angular** single-page application served by **nginx**, which also acts as a proxy to the backend.

> The interface is in Spanish. Strings live in the component templates (`src/app/**/*.html`), so translating it is a matter of editing them.

## What it shows

| Screen | Route | Content |
|---|---|---|
| Home | `/home` | Current weather and a card per section (house, Docker, devices, system, network, Grafana) with its key figures |
| House | `/casa` | A card per room (temperature, humidity, time since the last measurement), the house average and a history chart (temperature or humidity) |
| Weather | `/weather` | Current weather, daily forecast with hourly slots, animated background and icons ([Meteocons](https://github.com/basmilius/weather-icons), MIT) |
| Docker | `/docker` | The server's containers, with filter, search, and start/stop |
| Devices | `/devices` | Devices and their sensors: status and last measurement (`/sensors` redirects here) |
| Network | `/red` | Clients of the network (Omada), with search |
| Grafana | `/grafana` | CPU, memory, temperature and disks (from Prometheus) |
| System | `/system` | Uptime, usage gauges, backend version and refresh intervals |

- **A single request** to `GET /api/v1/dashboard/data` feeds every screen. It is repeated at the interval the backend indicates (`apiRefreshSeconds`, 30 s by default) and can be forced from the refresh button.
- **Installable (PWA):** manifest, service worker and an "Install app" button when the browser allows it.
- Stack: Angular 22 (standalone components, signals, lazy routes), TypeScript, Chart.js and plain SCSS.

## Architecture

```
Browser ──► nginx (bahguta-web) ──► Spring Boot backend (bahguta:8080)
              │  /                       static SPA
              │  /api/                   proxy, read only, no key
              └  /api/v1/system/docker/  proxy + password + X-API-Key
```

The `X-API-Key` that protects start/stop of containers is **never in the JavaScript**: nginx adds it, and only on `/api/v1/system/docker/`. That route also asks for a user and password (HTTP Basic auth).

## Development

Requirements: Node.js 24 and npm.

```bash
cd bahguta_spa
cp proxy.conf.example.json proxy.conf.json   # set the backend URL and your API key
npm ci
npm start                                    # http://localhost:4200
```

`proxy.conf.json` is in `.gitignore`: never commit it. In development the proxy adds the key to **every** `/api` request, so you can test the Docker buttons without nginx.

Other commands: `npm run build` (output in `dist/bahguta-web/browser`) and `npm test`. There are no tests yet (see the [roadmap](../docs/ROADMAP.md)).

## Deployment

The `Dockerfile` compiles the app (Node 24) and serves it with nginx. You need the backend and its Docker network `web` running first (see the [main README](../README.md)).

```bash
# 1. Files on the server (this example uses /opt/bahguta/web)
mkdir -p /opt/bahguta/web && cd /opt/bahguta/web
cp /path/to/repo/bahguta_spa/docker-compose.example.yml docker-compose.yml
cp /path/to/repo/bahguta_spa/.env.example .env
nano .env                       # BACKEND_URL and BAHGUTA_API_KEY (same value as the backend's)

# 2. User and password for the Docker control (nginx Basic auth)
printf 'admin:%s\n' "$(openssl passwd -apr1)" > htpasswd     # asks for the password

# 3. Build the image and start it
docker build -t bahguta-web:latest /path/to/repo/bahguta_spa
docker compose up -d
```

Then open `http://YOUR_SERVER/`. `htpasswd` must exist **before** starting: otherwise Docker creates a folder with that name and nginx fails.

If you use the GitLab pipeline (`.gitlab-ci.yml`), the `deploy web` job does steps 3 and the health check for you on every change in `bahguta_spa/`, with automatic rollback; steps 1 and 2 are done by hand once.

### Configuration

| Variable | Description |
|---|---|
| `BACKEND_URL` | Internal URL of the backend, for example `http://bahguta:8080`. nginx resolves the name again every few seconds, so recreating the backend does not require restarting the web |
| `BAHGUTA_API_KEY` | Same value as in the backend. Added by nginx to start/stop requests only |

## Security

- Like the rest of the project, it is meant for a **trusted local network**. **Do not expose it to the internet.**
- The password only protects `/api/v1/system/docker/`. The rest of `/api/` is proxied **without authentication**, as the backend has none: anyone who reaches the web can read, and also call the write endpoints of the API (settings, device registration, measurements).
- Basic auth travels in plain text over HTTP. If you need access from outside, put the web behind a reverse proxy with HTTPS and your own authentication.
- Never commit `.env`, `htpasswd` or `proxy.conf.json`.

## Structure

```
src/app/
├── core/       services (dashboard polling, Docker), models and helpers
├── layout/     shell: header, menu and footer
├── pages/      one folder per screen (lazy loaded)
└── shared/     reusable components (animated weather background)
nginx/          nginx template (proxy, cache and PWA headers)
public/         icons, manifest and Meteocons weather icons
```

## Credits

Weather icons by [Bas Milius](https://github.com/basmilius/weather-icons) under the MIT license (`public/meteocons/LICENSE-meteocons.txt`).
