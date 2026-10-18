#pragma once

#include <Arduino.h>

// Metricas de Grafana/Prometheus del servidor (vienen del backend).
// -1 = dato no disponible.
struct GrafanaInfo
{
    bool connected = false;

    // Discos en GB
    double rootTotalGb = -1;
    double rootUsedGb = -1;
    double rootFreeGb = -1;

    double storageTotalGb = -1;   // /srv/storage
    double storageUsedGb = -1;
    double storageFreeGb = -1;

    // Porcentajes
    double cpu = -1;
    double ram = -1;

    // Temperatura del servidor (C)
    double tempC = -1;
};
