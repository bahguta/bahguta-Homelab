package com.bahguta.bahguta_api.system.settings.service;

import com.bahguta.bahguta_api.system.settings.dto.SettingsData;
import com.bahguta.bahguta_api.system.settings.dto.SettingsUpdateRequest;

/**
 * Reglas puras (sin Spring ni BBDD): fusion de un cambio parcial sobre los
 * ajustes actuales y validacion de rangos. Se lanza IllegalArgumentException
 * con un mensaje legible si algun valor es invalido.
 */
final class SettingsLogic {

    static final int SENSOR_INTERVAL_MIN = 10,   SENSOR_INTERVAL_MAX = 3600;
    static final int WEATHER_REFRESH_MIN = 60,   WEATHER_REFRESH_MAX = 86400;
    static final int SYSTEM_REFRESH_MIN  = 1,    SYSTEM_REFRESH_MAX  = 3600;
    static final int MQTT_RECONNECT_MIN  = 1,    MQTT_RECONNECT_MAX  = 600;
    static final int BRIGHTNESS_MIN      = 0,    BRIGHTNESS_MAX      = 255;
    static final int SCREEN_TIMEOUT_MIN  = 0,    SCREEN_TIMEOUT_MAX  = 86400;
    static final int FORECAST_HOUR_MIN   = 0,    FORECAST_HOUR_MAX   = 23;
    static final int NAME_MAX = 64;

    private SettingsLogic() {}

    static SettingsData merge(SettingsData current, SettingsUpdateRequest req) {
        if (req == null) {
            return current;
        }

        return new SettingsData(
                range("apiRefreshSeconds", req.apiRefreshSeconds(), current.apiRefreshSeconds(),
                        SENSOR_INTERVAL_MIN, SENSOR_INTERVAL_MAX),
                range("weatherRefreshSeconds", req.weatherRefreshSeconds(), current.weatherRefreshSeconds(),
                        WEATHER_REFRESH_MIN, WEATHER_REFRESH_MAX),
                range("systemRefreshSeconds", req.systemRefreshSeconds(), current.systemRefreshSeconds(),
                        SYSTEM_REFRESH_MIN, SYSTEM_REFRESH_MAX),
                range("mqttReconnectSeconds", req.mqttReconnectSeconds(), current.mqttReconnectSeconds(),
                        MQTT_RECONNECT_MIN, MQTT_RECONNECT_MAX),
                name("deviceName", req.deviceName(), current.deviceName(), true),
                name("deviceLocation", req.deviceLocation(), current.deviceLocation(), false),
                range("brightness", req.brightness(), current.brightness(),
                        BRIGHTNESS_MIN, BRIGHTNESS_MAX),
                req.screenAlwaysOn() != null ? req.screenAlwaysOn() : current.screenAlwaysOn(),
                range("screenTimeoutSeconds", req.screenTimeoutSeconds(), current.screenTimeoutSeconds(),
                        SCREEN_TIMEOUT_MIN, SCREEN_TIMEOUT_MAX),
                range("forecastHour1", req.forecastHour1(), current.forecastHour1(),
                        FORECAST_HOUR_MIN, FORECAST_HOUR_MAX),
                range("forecastHour2", req.forecastHour2(), current.forecastHour2(),
                        FORECAST_HOUR_MIN, FORECAST_HOUR_MAX),
                range("forecastHour3", req.forecastHour3(), current.forecastHour3(),
                        FORECAST_HOUR_MIN, FORECAST_HOUR_MAX)
        );
    }

    private static int range(String field, Integer requested, int current, int min, int max) {
        if (requested == null) {
            return current;
        }
        if (requested < min || requested > max) {
            throw new IllegalArgumentException(
                    field + " debe estar entre " + min + " y " + max + " (recibido " + requested + ")");
        }
        return requested;
    }

    private static String name(String field, String requested, String current, boolean required) {
        if (requested == null) {
            return current;
        }
        String value = requested.trim();
        if (required && value.isEmpty()) {
            throw new IllegalArgumentException(field + " no puede estar vacio");
        }
        if (value.length() > NAME_MAX) {
            throw new IllegalArgumentException(field + " admite como maximo " + NAME_MAX + " caracteres");
        }
        return value;
    }
}
