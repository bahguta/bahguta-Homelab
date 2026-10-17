CREATE TABLE IF NOT EXISTS app_settings (
    id                      integer      PRIMARY KEY CHECK (id = 1),
    sensor_interval_seconds integer      NOT NULL CHECK (sensor_interval_seconds BETWEEN 10 AND 3600),
    weather_refresh_seconds integer      NOT NULL CHECK (weather_refresh_seconds BETWEEN 60 AND 86400),
    system_refresh_seconds  integer      NOT NULL CHECK (system_refresh_seconds BETWEEN 1 AND 3600),
    mqtt_reconnect_seconds  integer      NOT NULL CHECK (mqtt_reconnect_seconds BETWEEN 1 AND 600),
    device_name             varchar(64)  NOT NULL,
    device_location         varchar(64)  NOT NULL DEFAULT '',
    brightness              integer      NOT NULL CHECK (brightness BETWEEN 0 AND 255),
    screen_always_on        boolean      NOT NULL,
    screen_timeout_seconds  integer      NOT NULL CHECK (screen_timeout_seconds BETWEEN 0 AND 86400),
    updated_at              timestamptz  NOT NULL DEFAULT now(),
    version                 bigint       NOT NULL DEFAULT 0
);

INSERT INTO app_settings (
    id, sensor_interval_seconds, weather_refresh_seconds, system_refresh_seconds,
    mqtt_reconnect_seconds, device_name, device_location, brightness,
    screen_always_on, screen_timeout_seconds)
VALUES (1, 30, 900, 5, 10, 'ESP32-PlantaAlta', 'Oficina Principal', 200, true, 0)
ON CONFLICT (id) DO NOTHING;
