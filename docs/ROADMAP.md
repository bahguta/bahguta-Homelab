# Roadmap

Open ideas for the project. They are not commitments and have no dates: they are a starting point for anyone who wants to collaborate. If one interests you, open an issue to discuss it.

## Undecided

- **Battery level** of each sensor, measured on the ESP32-C3 and shown on the tablet (Devices and House screens).
- **Intervals editable from the tablet**: sensor measurement interval and REST refresh.
- **Unify Device and Sensor** in the interface and fix devices that show as *online* when they no longer are.

## Security

- API authentication for the rest of the endpoints (today only the Docker start/stop ones require a key); for example, one key per device and another for the tablet.
- HTTPS between the tablet and the backend, so keys do not travel in plain text.
- Make the devices' `secret_key` a real secret rather than the room name.
- Authentication on the MQTT broker if it ends up being used.

## Possible extensions

- **MQTT:** use the already prepared broker to publish measurements, for example towards Home Assistant.
- More sensor types (BME280, soil, rain, PIR, relays): the database schema already supports them.
- More interface languages on the tablet.
- Photos and screenshots in the documentation, and a step-by-step build guide.
- Automated backend tests and a firmware build pipeline.

## Done

- REST backend with an aggregated dashboard: a single request every 30 s for the whole tablet.
- Tablet interface with house, weather, devices, sensors, Docker, network, Grafana, system and settings screens.
- Animated splash, smooth screen transitions and a backlight fade on sleep and wake.
- Project version shown on the tablet, taken from the backend.
- Continuous deployment with GitLab CI and automatic rollback.
- Docker control hardened: API key on start/stop, allow-list of containers and a restricted Docker socket proxy instead of mounting `docker.sock`.
