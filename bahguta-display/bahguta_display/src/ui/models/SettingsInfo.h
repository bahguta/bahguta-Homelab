#pragma once

#include <Arduino.h>

struct SettingsInfo
{
    // -------------------------
    // WiFi
    // -------------------------

    String wifiSsid;
    String wifiPassword;

    bool configured = false;


    // -------------------------
    // Servicios
    // -------------------------

    uint32_t apiRefreshSeconds = 60;
    uint32_t weatherRefreshSeconds = 300;
    uint32_t systemRefreshSeconds = 10;

    uint32_t mqttReconnectSeconds = 10;

    // Horas (0-23) de las 3 franjas del pronostico en WeatherScreen (viene del backend)
    uint8_t forecastHour[3] = { 9, 14, 19 };


    // -------------------------
    // Dispositivo
    // -------------------------

    String deviceName = "bahguta-display";

    String deviceLocation = "";

    bool autoStart = true;


    // -------------------------
    // Display
    // -------------------------

    uint8_t brightness = 100;

    bool screenAlwaysOn = true;

    uint32_t screenTimeoutSeconds = 0;


    // -------------------------
    // Backend
    // -------------------------
    String backendUrl = "http://bahguta.casa.lan";

    String timezone = "CET-1CEST,M3.5.0,M10.5.0/3";

    uint32_t refreshInterval = 30000;

    // Segundos sin tocar la tablet antes de apagar la pantalla (0 = nunca).
    // Solo se guarda en la tablet (NVS); no va al backend.
    uint32_t inactivitySeconds = 60;





    
};