#pragma once

#include <Arduino.h>
#include <vector>

// Un equipo conectado al router (viene del backend)
struct RedClientInfo
{
    String name;        // hostname o alias
    String ip;
    String mac;
    String connection;  // "wifi" | "cable"
    bool online = true;
};

struct RedInfo
{
    bool connected = false;      // el backend devolvio datos de red
    int total = 0;               // equipos conectados ahora
    std::vector<RedClientInfo> clients;
};
