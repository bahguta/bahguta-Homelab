#include "ApiClient.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "../include/Config.h"

void ApiClient::heartbeat()
{
    HTTPClient http;

    http.begin(String(BACKEND_URL) + "/api/v1/devices/heartbeat");

    http.addHeader("Content-Type", "application/json");

    JsonDocument json;

    json["deviceId"] = storage.getDeviceUuid();
    json["firmwareVersion"] = FIRMWARE_VERSION;
    json["ipAddress"] = WiFi.localIP().toString();
    json["macAddress"] = WiFi.macAddress();
    json["rssi"] = WiFi.RSSI();
    json["freeHeap"] = ESP.getFreeHeap();
    json["uptime"] = millis() / 1000;
    json["configVersion"] = 1;

    String body;

    serializeJson(json, body);

    Serial.println("======== HEARTBEAT ========");
    Serial.println(body);
    Serial.println("---------------------------");
    int httpCode = http.POST(body);

    Serial.print("Heartbeat HTTP Code: ");
    Serial.println(httpCode);
    Serial.println("---------------------------");
    
    if (httpCode > 0)
    {
        String response = http.getString();

        Serial.println(response);
        processConfigurationResponse(response);
    } 

    http.end();
    Serial.println("===========================");
}
