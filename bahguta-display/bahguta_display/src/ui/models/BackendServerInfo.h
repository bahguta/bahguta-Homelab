#pragma once

#include <Arduino.h>

struct BackendServerInfo
{
    bool connected = false;

    double cpu = 0.0;
    double ram = 0.0;
    double uptime = 0.0;

    // Version del proyecto (la del pom del backend), ej. "1.0.0".
    // Se guarda en la NVS para mostrarla ya en el arranque.
    char version[16] = "";
};