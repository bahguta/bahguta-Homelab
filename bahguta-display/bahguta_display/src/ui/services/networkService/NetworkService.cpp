#include "NetworkService.h"

#include <WiFi.h>
#include "../ServiceManager.h"

void NetworkService::init()
{
    WiFi.mode(WIFI_STA);

    WiFi.setSleep(false);

    connect();
}

void NetworkService::update()
{
    refresh();

    if(currentNetwork.connected)
    {
        disconnectedSince = 0;
        return;
    }

    if (disconnectedSince == 0)
    {
        disconnectedSince = millis();
    }
    else if (millis() - disconnectedSince >= STUCK_REBOOT_TIMEOUT)
    {
        Serial.println("WiFi lleva demasiado tiempo caida (posible fallo del companion chip) -> reinicio.");
        ESP.restart();
    }

    if(millis() - lastAttempt < RETRY_TIME)
        return;

    connect();
}

const NetworkInfo& NetworkService::network() const
{
    return currentNetwork;
}

void NetworkService::reconnect()
{
    // Se llama desde la UI (boton Guardar de Settings), mientras
    // que update()/connect() corren en el runtime task. Usamos el
    // mismo mutex que ya protege el resto de datos compartidos.

    ServiceManager::lockData();

    WiFi.disconnect();
    lastAttempt = 0;

    connect();

    ServiceManager::unlockData();
}

void NetworkService::connect()
{
     Serial.print("Connecting WIFI ...");

    lastAttempt = millis();

    const SettingsInfo& settings =
        ServiceManager::settings().settings();



    if (settings.wifiSsid.isEmpty())
    {
        Serial.print(" NO HAY WIFI");
        return;
    }

    WiFi.begin(
        settings.wifiSsid.c_str(),
        settings.wifiPassword.c_str());
}

void NetworkService::refresh()
{
    currentNetwork.connected =
        WiFi.status() == WL_CONNECTED;

    if(!currentNetwork.connected)
        return;
        
    currentNetwork.ssid =
        WiFi.SSID();

    currentNetwork.ip =
        WiFi.localIP().toString();

    currentNetwork.rssi =
        WiFi.RSSI();
}