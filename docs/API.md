# REST API

Base URL: `http://YOUR_SERVER:8080`. Interactive documentation (Swagger UI) is at `/swagger`.

> The API has **no authentication**: use it only on a trusted local network (see the security notice in the [README](../README.md)).

## Endpoints

### Dashboard (what the tablet polls)

| Method | Path | Description |
|---|---|---|
| GET | `/api/v1/dashboard/data` | Aggregated JSON with everything the tablet needs. Includes a `version` field with the project version |
| GET | `/api/v1/dashboard/latest` | Latest measurements |
| GET | `/api/v1/dashboard/all` | Full dashboard view |

### Devices

| Method | Path | Description |
|---|---|---|
| POST | `/api/v1/devices/register` | Registers a device (called by the sensor on boot) |
| POST | `/api/v1/devices/heartbeat` | Device keep-alive |
| GET | `/api/v1/devices/list` | List of devices |
| GET | `/api/v1/devices/device/{deviceId}` | Device details |
| GET | `/api/v1/devices/error` | Devices with errors |

### Sensors

| Method | Path | Description |
|---|---|---|
| GET | `/api/v1/sensors/list` | List of sensors |
| GET | `/api/v1/sensors/device/{deviceId}` | Sensors of a device |
| GET | `/api/v1/sensors/sensor/{sensorId}` | Sensor details |
| PUT | `/api/v1/sensors/sensor/{sensorId}` | Updates a sensor's configuration |

### Measurements

| Method | Path | Description |
|---|---|---|
| POST | `/api/v1/measurements/register` | Stores a measurement. Body: `sensorId` (UUID), `sensorName`, and `temperature`, `humidity`, `pressure` or `value` depending on the sensor |
| GET | `/api/v1/measurements/{sensorId}/latest` | Latest measurement of a sensor |
| GET | `/api/v1/measurements/{sensorId}/history?from=...&to=...` | History between two dates (`LocalDateTime`) |
| GET | `/api/v1/measurements/summary?hours=24&points=24` | Compact history for the tablet chart: per-room averages by time bucket |

### Weather

| Method | Path | Description |
|---|---|---|
| GET | `/api/v1/weather` | Current weather (OpenWeather) |
| GET | `/api/v1/weather/5dias` | 5-day forecast |

### System

| Method | Path | Description |
|---|---|---|
| GET | `/api/v1/system/server` | Server status |
| GET | `/api/v1/system/docker/container/list` | Docker containers |
| POST | `/api/v1/system/docker/container/{id}/start` | Starts a container |
| POST | `/api/v1/system/docker/container/{id}/stop` | Stops a container |
| — | `/api/v1/settings` | Application settings (weather time slots, etc.) |

The `docker` endpoints run real commands on the server: **do not expose them outside your network**.

#### Authentication of the Docker endpoints

The two `POST` endpoints (`start` and `stop`) require the `X-API-Key` header with the value of `BAHGUTA_API_KEY`. The `GET` list does not need it, and neither does the rest of the API.

```bash
curl -X POST -H "X-API-Key: $BAHGUTA_API_KEY" \
  http://YOUR_SERVER:8080/api/v1/system/docker/container/grafana/stop
```

`{id}` accepts the container id or its name, but the container's **name** must be in `DOCKER_ALLOWED`.

| Code | Cause |
|---|---|
| `401` | `X-API-Key` is missing or wrong |
| `403` | The container is not in `DOCKER_ALLOWED` |

See [NETWORK.md](NETWORK.md#docker-containers) for how the access to Docker is protected.

## Examples

```bash
# Full dashboard (the one the tablet uses)
curl http://YOUR_SERVER:8080/api/v1/dashboard/data

# Send a measurement manually
curl -X POST http://YOUR_SERVER:8080/api/v1/measurements/register \
  -H 'Content-Type: application/json' \
  -d '{"sensorId":"00000000-0000-0000-0000-000000000000","sensorName":"Kitchen","temperature":21.5,"humidity":48}'

# Last 24 hours, 24 points
curl "http://YOUR_SERVER:8080/api/v1/measurements/summary?hours=24&points=24"
```

## Database and network

- The schema is created by Flyway on startup (`src/main/resources/db/migration`).
- `compose/postgres.yml` runs PostgreSQL on the external Docker network `web`; the backend (`spring/docker-compose.yml`) uses the same network and connects to the host `postgres` (configurable with `DB_URL`).
- MQTT: the broker in `compose/mqtt.yml` is optional and **not used yet**; the backend neither publishes nor reads anything over MQTT.
- The scripts in `scripts/` assume the project lives in `/opt/bahguta`.
