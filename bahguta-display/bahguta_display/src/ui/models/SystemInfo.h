#pragma once

#include <Arduino.h>

struct SystemInfo
{
    bool backendOnline = false;
    bool internetAvailable = false;
    bool mqttConnected = false;

    uint32_t uptime = 0;

    uint8_t cpuLoad = 0;
    uint8_t memoryUsage = 0;

    int battery = -1;
    bool charging = false;

    // ========================================================
    // SERVER
    // ========================================================

    double cpu = 0.0;
    double ram = 0.0;

    // ========================================================
    // TABLET
    // ========================================================

    //double tabletCpu = 0.0;
};