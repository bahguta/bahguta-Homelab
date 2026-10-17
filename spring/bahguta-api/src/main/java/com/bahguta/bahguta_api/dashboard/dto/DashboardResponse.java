package com.bahguta.bahguta_api.dashboard.dto;

import java.util.List;

import com.fasterxml.jackson.annotation.JsonInclude;

import lombok.Builder;

/**
 * Respuesta agregada de GET /api/v1/dashboard?cfgVer=N
 *
 * - Timestamps en EPOCH SEGUNDOS.
 * - Cada seccion va envuelta en Section<T>: un fallo parcial no tira el resto.
 * - "config" es null (y se omite del JSON) si cfgVer del tablet == cfgVersion actual.
 */
@Builder
@JsonInclude(JsonInclude.Include.NON_NULL)
public record DashboardResponse(
        boolean success,
        long ts,
        int cfgVersion,
        Section<WeatherData> weather,
        Section<List<Room>> rooms,
        Summary summary,
        Section<List<DockerContainer>> docker,
        Section<List<Device>> devices,
        Section<List<Sensor>> sensors,
        Section<SystemStats> system,
        Section<RedData> red,
        Section<GrafanaData> grafana,
        TabletConfig config,
        String version
) {

    /** ok=false => data=null y el tablet conserva lo ultimo que pinto. */
    public record Section<T>(boolean ok, Long updatedAt, T data) {
        public static <T> Section<T> ok(T data, Long updatedAt) {
            return new Section<>(true, updatedAt, data);
        }
        public static <T> Section<T> failed() {
            return new Section<>(false, null, null);
        }
    }

    // ---------- WEATHER ----------
    public record WeatherData(CurrentWeather current, List<ForecastDay> forecast) {}

    public record CurrentWeather(
            String location,
            Double temperature,
            Double feelsLike,
            Double minTemperature,
            Double maxTemperature,
            Integer humidity,
            Integer pressure,
            String description,
            String icon,
            Double windSpeed,
            Integer windDirection,
            Integer clouds,
            Long sunrise,
            Long sunset,
            Integer timezone
    ) {}

    /** Un dia agregado a partir de los tramos de 3 h (ver WeatherForecastAggregator). */
    public record ForecastDay(
            String date,          // yyyy-MM-dd local; el tablet calcula el dia de la semana
            double min,
            double max,
            String icon,          // siempre variante diurna (xxd)
            String description,
            double rainMm,
            List<ForecastHour> hours   // franjas del dia (hasta 3: 08:00, 14:00 y 20:00)
    ) {}

    /**
     * Una franja del dia: hora de referencia (8, 14 o 20), minima y maxima del periodo
     * y el icono de OpenWeather del tramo mas cercano a esa hora (ej. "10n").
     */
    public record ForecastHour(int hour, double min, double max, String icon) {}

    // ---------- CASA: una entrada por habitacion (device.location) ----------
    public record Room(
            String room,
            Double temperature,   // null si no hay medicion valida
            Double humidity,
            Long lastMeasurement,
            boolean stale
    ) {}

    // ---------- HOME ----------
    public record Summary(
            Double avgTemperature,
            Double avgHumidity,
            Counter docker,
            Counter devices,
            Counter sensors
    ) {}

    /** docker usa "running"; devices y sensors usan "online". El otro va null y se omite. */
    public record Counter(Integer running, Integer online, int total) {
        public static Counter docker(int running, int total) { return new Counter(running, null, total); }
        public static Counter online(int online, int total)  { return new Counter(null, online, total); }
    }

    // ---------- DOCKER (sin image ni ports) ----------
    public record DockerContainer(String id, String name, String state, String status) {}

    // ---------- DEVICES (sin secretKey) ----------
    public record Device(
            String id,
            String name,
            String description,
            String hardwareModel,
            String location,
            String status,
            String deviceType,
            String ip,
            String mac,
            String fw
    ) {}

    // ---------- SENSORS (config + ultima medicion) ----------
    public record Sensor(
            String id,
            String deviceId,
            String name,
            String type,
            boolean enabled,
            Integer interval,
            Double temperature,
            Double humidity,
            Long lastMeasurement,
            boolean stale
    ) {}

    // ---------- SYSTEM ----------
    public record SystemStats(double cpu, double ram, double uptime) {}

    // ---------- RED (equipos conectados al router) ----------
    public record RedData(int total, List<RedClient> clients) {}

    /** type: "wifi" | "cable" ("" si no se sabe) */
    public record RedClient(String name, String ip, String mac, String type) {}

    // ---------- GRAFANA (metricas del servidor; null = sin dato) ----------
    public record GrafanaData(
            Double rootTotalGb,
            Double rootUsedGb,
            Double rootFreeGb,
            Double storageTotalGb,
            Double storageUsedGb,
            Double storageFreeGb,
            Double cpu,
            Double ram,
            Double tempC
    ) {}

    // ---------- CONFIG (solo si cambio) ----------
    public record TabletConfig(
            int apiRefreshSeconds,
            int weatherRefreshSeconds,
            int systemRefreshSeconds,
            int mqttReconnectSeconds,
            String deviceName,
            String deviceLocation,
            int brightness,
            boolean screenAlwaysOn,
            int screenTimeoutSeconds,
            int forecastHour1,
            int forecastHour2,
            int forecastHour3
    ) {}
}
