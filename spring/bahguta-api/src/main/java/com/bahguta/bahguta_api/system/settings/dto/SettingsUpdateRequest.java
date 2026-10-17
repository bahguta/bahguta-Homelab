package com.bahguta.bahguta_api.system.settings.dto;

/**
 * Cuerpo de POST / PUT / PATCH /api/v1/settings.
 *
 * Todos los campos son opcionales: lo que venga a null NO se modifica. Asi el
 * mismo endpoint sirve para un cambio parcial ({"apiRefreshSeconds":60}) y
 * para el objeto completo que envia la tablet.
 */
public record SettingsUpdateRequest(
        Integer apiRefreshSeconds,      // cada cuanto miden los sensores
        Integer weatherRefreshSeconds,
        Integer systemRefreshSeconds,
        Integer mqttReconnectSeconds,
        String deviceName,
        String deviceLocation,
        Integer brightness,
        Boolean screenAlwaysOn,
        Integer screenTimeoutSeconds,
        Integer forecastHour1,          // horas (0-23) de las franjas del pronostico
        Integer forecastHour2,
        Integer forecastHour3
) {}
