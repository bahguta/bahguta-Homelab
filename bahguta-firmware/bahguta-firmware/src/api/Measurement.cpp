#include "ApiClient.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "../include/Config.h"

void ApiClient::registerMeasurement(const Measurement& measurement)
{
    HTTPClient http;

    http.begin(String(BACKEND_URL) + "/api/v1/measurements/register");

    http.addHeader("Content-Type", "application/json");

    JsonDocument json;

    json["sensorId"] = storage.getSensorUuid("temperature");
    json["sensorName"] = "Temperature";

    if (!isnan(measurement.temperature))
        json["temperature"] = measurement.temperature;

    if (!isnan(measurement.humidity))
        json["humidity"] = measurement.humidity;

    if (!isnan(measurement.pressure))
        json["pressure"] = measurement.pressure;

    if (!isnan(measurement.value))
        json["value"] = measurement.value;

    String body;

    serializeJson(json, body);

    
    Serial.println("======= MEASUREMENT =======");
    
    Serial.println(body);
    Serial.println("---------------------------");
    int httpCode = http.POST(body);

    Serial.print("Measurement HTTP Code: ");
    Serial.println(httpCode);
    Serial.println("---------------------------");
    
    if (httpCode > 0)
    {
        Serial.println(http.getString());
    }

    http.end();
    Serial.println("===========================");
}