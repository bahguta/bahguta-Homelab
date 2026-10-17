-- ===========================================================
-- ENUMS
-- ===========================================================

CREATE TYPE device_status AS ENUM (
    'ONLINE',
    'OFFLINE',
    'ERROR'
);

CREATE TYPE device_type AS ENUM (
    'ESP32',
    'ESP32_C3',
    'ESP32_S3',
    'ESP32_P4',
    'UNKNOWN'
);

CREATE TYPE sensor_type AS ENUM (
    'DHT11',
    'DHT22',
    'BME280',
    'MQ2',
    'SOIL',
    'RAIN',
    'RELAY',
    'PIR',
    'UNKNOWN'
);



-- ===========================================================
-- DEVICE CONFIG
-- ===========================================================

CREATE TABLE device_config
(
    id UUID PRIMARY KEY,

    version BIGINT NOT NULL DEFAULT 1,

    firmware_version VARCHAR(50),

    ip_address VARCHAR(45),

    mac_address VARCHAR(17),

    mqtt_host VARCHAR(255),
    mqtt_port INTEGER,
    mqtt_topic VARCHAR(255),

    success BOOLEAN NOT NULL DEFAULT TRUE,
    heartbeat_seconds INTEGER,

    created_at TIMESTAMP NOT NULL,
    updated_at TIMESTAMP NOT NULL
);

CREATE INDEX idx_device_config_mac
ON device_config(mac_address);

CREATE INDEX idx_device_config_version
ON device_config(version);



-- ===========================================================
-- SENSOR CONFIG
-- ===========================================================

CREATE TABLE sensor_config
(
    id UUID PRIMARY KEY,

    version BIGINT NOT NULL DEFAULT 1,

    enabled BOOLEAN NOT NULL DEFAULT TRUE,

    interval INTEGER NOT NULL,

    created_at TIMESTAMP NOT NULL,
    updated_at TIMESTAMP NOT NULL
);

CREATE INDEX idx_sensor_config_version
ON sensor_config(version);




-- ===========================================================
-- DEVICE
-- ===========================================================

CREATE TABLE device
(
    id UUID PRIMARY KEY,

    name VARCHAR(100) NOT NULL,

    description VARCHAR(500),

    hardware_model VARCHAR(100),

    location VARCHAR(100),

    status device_status NOT NULL,

    enabled BOOLEAN NOT NULL,

    secret_key VARCHAR(64),

    last_seen TIMESTAMP,

    device_type device_type NOT NULL,

    device_config_id UUID NOT NULL,

    created_at TIMESTAMP NOT NULL,

    updated_at TIMESTAMP NOT NULL,

    CONSTRAINT fk_device_config
        FOREIGN KEY (device_config_id)
        REFERENCES device_config(id)
);

CREATE INDEX idx_device_status
ON device(status);

CREATE INDEX idx_device_enabled
ON device(enabled);

CREATE INDEX idx_device_last_seen
ON device(last_seen);

CREATE INDEX idx_device_location
ON device(location);



-- ===========================================================
-- SENSOR
-- ===========================================================

CREATE TABLE sensor
(
    id UUID PRIMARY KEY,

    device_id UUID NOT NULL,

    sensor_config_id UUID NOT NULL,

    type sensor_type NOT NULL,

    name VARCHAR(50) NOT NULL,

    enabled BOOLEAN NOT NULL DEFAULT TRUE,

    created_at TIMESTAMP NOT NULL,

    updated_at TIMESTAMP NOT NULL,

    CONSTRAINT fk_sensor_device
        FOREIGN KEY (device_id)
        REFERENCES device(id)
        ON DELETE CASCADE,

    CONSTRAINT fk_sensor_config
        FOREIGN KEY (sensor_config_id)
        REFERENCES sensor_config(id)
);

CREATE INDEX uk_sensor_device_name
ON sensor(device_id,name);

CREATE INDEX idx_sensor_device
ON sensor(device_id);

CREATE INDEX idx_sensor_type
ON sensor(type);

CREATE INDEX idx_sensor_enabled
ON sensor(enabled);


-- ===========================================================
-- MEASUREMENT
-- ===========================================================

CREATE TABLE measurement
(
    id UUID PRIMARY KEY,

    sensor_id UUID NOT NULL,

    temperature DOUBLE PRECISION,

    humidity DOUBLE PRECISION,

    pressure DOUBLE PRECISION,

    valor DOUBLE PRECISION,

    created_at TIMESTAMP NOT NULL,

    updated_at TIMESTAMP NOT NULL,

    CONSTRAINT fk_measurement_sensor
        FOREIGN KEY(sensor_id)
        REFERENCES sensor(id)
        ON DELETE CASCADE
);

CREATE INDEX idx_measurement_sensor
ON measurement(sensor_id);

CREATE INDEX idx_measurement_date
ON measurement(created_at);

CREATE INDEX idx_measurement_sensor_date
ON measurement(sensor_id, created_at DESC);

