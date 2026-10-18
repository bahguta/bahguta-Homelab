#include "ApiClient.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include "../include/Config.h"

void ApiClient::registerDevice()
{
    HTTPClient http;

    http.begin(String(BACKEND_URL) + "/api/v1/devices/register");

    http.addHeader("Content-Type", "application/json");

    JsonDocument json;

    json["id"] = storage.getDeviceUuid();
    json["name"] = DEVICE_NAME;
    json["description"] = DEVICE_DESCRIPTION;
    json["type"] = DEVICE_TYPE;
    json["firmwareVersion"] = FIRMWARE_VERSION;
    json["macAddress"] = WiFi.macAddress();
    json["ipAddress"] = WiFi.localIP().toString();
    json["hardwareModel"] = HARDWARE_MODEL;
    json["location"] = DEVICE_LOCATION;
    json["secretKey"] = SECRET_KEY;

    JsonArray sensors = json["sensors"].to<JsonArray>();

    JsonObject sensor = sensors.add<JsonObject>();

    sensor["id"] = storage.getSensorUuid("temperature");
    sensor["deviceId"] = storage.getDeviceUuid();
    sensor["name"] = "Temperature";
    sensor["type"] = "DHT22";

    String body;

    serializeJson(json, body);

    Serial.println("===== REGISTER REQUEST ====");
    
    Serial.println(body);
    Serial.println("---------------------------");
    int httpCode = http.POST(body);

    Serial.print("HTTP Code: ");
    Serial.println(httpCode);
    Serial.println("---------------------------");

    if (httpCode > 0)
    {
        String response = http.getString();
        processConfigurationResponse(response);
        storage.setRegistered(true);
    }

    http.end();
    Serial.println("===========================");
}