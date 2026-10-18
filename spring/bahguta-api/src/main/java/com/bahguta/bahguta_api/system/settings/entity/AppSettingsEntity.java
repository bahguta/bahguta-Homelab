package com.bahguta.bahguta_api.system.settings.entity;

import java.time.Instant;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.Id;
import jakarta.persistence.Table;
import jakarta.persistence.Version;

import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

/**
 * Configuracion global (una unica fila, id = 1).
 *
 * Los nombres de columna son explicitos: "sensor_interval_seconds" es lo que
 * el JSON y la tablet llaman "apiRefreshSeconds" (cada cuanto miden los sensores).
 */
@Getter
@Setter
@NoArgsConstructor
@Entity
@Table(name = "app_settings")
public class AppSettingsEntity {

    public static final int SINGLETON_ID = 1;

    @Id
    private Integer id = SINGLETON_ID;

    @Column(name = "sensor_interval_seconds", nullable = false)
    private int sensorIntervalSeconds;

    @Column(name = "weather_refresh_seconds", nullable = false)
    private int weatherRefreshSeconds;

    @Column(name = "system_refresh_seconds", nullable = false)
    private int systemRefreshSeconds;

    @Column(name = "mqtt_reconnect_seconds", nullable = false)
    private int mqttReconnectSeconds;

    @Column(name = "device_name", nullable = false, length = 64)
    private String deviceName;

    @Column(name = "device_location", nullable = false, length = 64)
    private String deviceLocation = "";

    @Column(name = "brightness", nullable = false)
    private int brightness;

    @Column(name = "screen_always_on", nullable = false)
    private boolean screenAlwaysOn;

    @Column(name = "screen_timeout_seconds", nullable = false)
    private int screenTimeoutSeconds;

    /** Horas de referencia (0-23) de las 3 franjas del pronostico en WeatherScreen. */
    @Column(name = "forecast_hour_1", nullable = false)
    private int forecastHour1 = 9;

    @Column(name = "forecast_hour_2", nullable = false)
    private int forecastHour2 = 14;

    @Column(name = "forecast_hour_3", nullable = false)
    private int forecastHour3 = 19;

    @Column(name = "updated_at", nullable = false)
    private Instant updatedAt = Instant.now();

    /** Bloqueo optimista: dos actualizaciones simultaneas no se pisan en silencio. */
    @Version
    private Long version;
}
