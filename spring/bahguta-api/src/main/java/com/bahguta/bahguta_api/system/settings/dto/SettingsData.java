package com.bahguta.bahguta_api.system.settings.dto;

import lombok.Builder;


@Builder
public record SettingsData(
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
