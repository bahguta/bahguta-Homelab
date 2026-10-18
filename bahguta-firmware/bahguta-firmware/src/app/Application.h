#pragma once

#include "../wifi/WifiManager.h"
#include "../sensors/TemperatureSensor.h"
#include "../api/ApiClient.h"
#include "../storage/Storage.h"
#include "../power/PowerManager.h"

/*
 * El dispositivo hace un unico ciclo por cada vez que despierta:
 * conectar wifi, heartbeat (que de paso trae la configuracion,
 * incluido cada cuanto volver a despertar), leer temperatura y
 * humedad, enviarlas, y volver a deep sleep. No hay una fase de
 * "loop" continua -- por eso esta clase solo tiene setup().
 */

class Application
{
public:

    void setup();

private:

    WifiManager wifiManager;
    PowerManager powerManager;
    TemperatureSensor temperatureSensor;
    ApiClient apiClient;
    Storage storage;
};
