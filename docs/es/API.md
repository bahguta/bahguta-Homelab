# API REST

Base: `http://TU_SERVIDOR:8080`. La documentación interactiva (Swagger UI) está en `/swagger`.

> La API **no tiene autenticación**: úsala solo en una red local de confianza (ver el aviso de seguridad del [README](../../README.es.md)).

## Endpoints

### Dashboard (lo que consultan la tablet y la web)

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/v1/dashboard/data` | JSON agregado con todo lo que necesita la tablet. Incluye el campo `version` con la versión del proyecto |
| GET | `/api/v1/dashboard/latest` | Últimas mediciones |
| GET | `/api/v1/dashboard/all` | Vista completa del dashboard |

### Dispositivos

| Método | Ruta | Descripción |
|---|---|---|
| POST | `/api/v1/devices/register` | Registro de un dispositivo (lo llama el sensor al arrancar) |
| POST | `/api/v1/devices/heartbeat` | Señal de vida del dispositivo |
| GET | `/api/v1/devices/list` | Lista de dispositivos |
| GET | `/api/v1/devices/device/{deviceId}` | Detalle de un dispositivo |
| GET | `/api/v1/devices/error` | Dispositivos con errores |

### Sensores

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/v1/sensors/list` | Lista de sensores |
| GET | `/api/v1/sensors/device/{deviceId}` | Sensores de un dispositivo |
| GET | `/api/v1/sensors/sensor/{sensorId}` | Detalle de un sensor |
| PUT | `/api/v1/sensors/sensor/{sensorId}` | Actualiza la configuración de un sensor |

### Mediciones

| Método | Ruta | Descripción |
|---|---|---|
| POST | `/api/v1/measurements/register` | Registra una medición. Cuerpo: `sensorId` (UUID), `sensorName`, y `temperature`, `humidity`, `pressure` o `value` según el sensor |
| GET | `/api/v1/measurements/{sensorId}/latest` | Última medición de un sensor |
| GET | `/api/v1/measurements/{sensorId}/history?from=...&to=...` | Histórico entre dos fechas (`LocalDateTime`) |
| GET | `/api/v1/measurements/summary?hours=24&points=24` | Histórico compacto para el gráfico de la tablet: medias por habitación en tramos de tiempo |

### Tiempo

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/v1/weather` | Tiempo actual (OpenWeather) |
| GET | `/api/v1/weather/5dias` | Previsión a 5 días |

### Sistema

| Método | Ruta | Descripción |
|---|---|---|
| GET | `/api/v1/system/server` | Estado del servidor |
| GET | `/api/v1/system/docker/container/list` | Contenedores Docker |
| POST | `/api/v1/system/docker/container/{id}/start` | Arranca un contenedor |
| POST | `/api/v1/system/docker/container/{id}/stop` | Para un contenedor |
| POST / PUT / PATCH | `/api/v1/settings` | Actualiza los ajustes de la aplicación (franjas horarias del tiempo, etc.). No hay `GET`: los valores actuales llegan en el campo `config` de `/api/v1/dashboard/data` |

Los endpoints de `docker` ejecutan comandos reales en el servidor: **no los expongas fuera de tu red**.

#### Autenticación de los endpoints de Docker

Los dos `POST` (`start` y `stop`) exigen la cabecera `X-API-Key` con el valor de `BAHGUTA_API_KEY`. El `GET` de la lista no la necesita, ni tampoco el resto de la API.

```bash
curl -X POST -H "X-API-Key: $BAHGUTA_API_KEY" \
  http://TU_SERVIDOR:8080/api/v1/system/docker/container/grafana/stop
```

`{id}` acepta el id del contenedor o su nombre, pero el **nombre** del contenedor tiene que estar en `DOCKER_ALLOWED`.

| Código | Causa |
|---|---|
| `401` | Falta `X-API-Key` o es incorrecta |
| `403` | El contenedor no está en `DOCKER_ALLOWED` |

En [NETWORK.md](NETWORK.md#docker-contenedores) se explica cómo se protege el acceso a Docker.

La web no envía la clave desde el navegador: su nginx añade `X-API-Key` y pide usuario y contraseña en `/api/v1/system/docker/`. Todo lo demás bajo `/api/` se reenvía tal cual, incluidos los endpoints de escritura (`settings`, `devices/register`, `measurements/register`).

## Ejemplos

```bash
# Dashboard completo (el que usa la tablet)
curl http://TU_SERVIDOR:8080/api/v1/dashboard/data

# Enviar una medición manualmente
curl -X POST http://TU_SERVIDOR:8080/api/v1/measurements/register \
  -H 'Content-Type: application/json' \
  -d '{"sensorId":"00000000-0000-0000-0000-000000000000","sensorName":"Cocina","temperature":21.5,"humidity":48}'

# Histórico de las últimas 24 h, 24 puntos
curl "http://TU_SERVIDOR:8080/api/v1/measurements/summary?hours=24&points=24"
```

## Base de datos y red

- El esquema lo crea Flyway al arrancar (`src/main/resources/db/migration`).
- `compose/postgres.yml` levanta PostgreSQL en la red externa de Docker `web`; el backend (`spring/docker-compose.yml`) usa la misma red y se conecta al host `postgres` (configurable con `DB_URL`).
- MQTT: el broker de `compose/mqtt.yml` es opcional y **no se usa todavía**; el backend no publica ni lee nada por MQTT.
- Los scripts de `scripts/` asumen que el proyecto está en `/opt/bahguta`.
