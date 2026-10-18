#include "WifiManager.h"

#include <Arduino.h>

#include "../../include/Config.h"

void WifiManager::setup()
{
    Serial.println("Connecting to WiFi...");

    WiFi.mode(WIFI_STA);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Connected!");
    }
    else
    {
        Serial.println("Connection failed");
    }

    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

bool WifiManager::isConnected() const
{
    return WiFi.status() == WL_CONNECTED;
}

String WifiManager::getIpAddress() const
{
    return WiFi.localIP().toString();
}