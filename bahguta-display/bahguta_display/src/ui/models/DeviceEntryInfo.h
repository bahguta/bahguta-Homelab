#pragma once

#include <Arduino.h>

struct DeviceEntryInfo
{
    String id;
    String name;
    String description;
    String hardwareModel;
    String location;
    String deviceType;

    String ipAddress;
    String macAddress;
    String firmwareVersion;

    bool online = false;

    // ---- Sensor asociado (unificado con el device en la tablet) ----
    bool hasSensor = false;        // el backend envio un sensor para este device
    String sensorId;
    String sensorType;             // "DHT22"
    bool sensorEnabled = false;
    int sensorInterval = 0;        // segundos

    bool hasReading = false;       // temperature valida
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool humidityValid = false;
    bool stale = true;             // ultima medicion antigua (sensor sin reportar)
};