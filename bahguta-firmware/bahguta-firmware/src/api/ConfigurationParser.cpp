#include "ApiClient.h"

#include <Arduino.h>
#include <ArduinoJson.h>

void ApiClient::processConfigurationResponse(const String& response)
{
    JsonDocument json;

    DeserializationError error = deserializeJson(json, response);

    if (error)
    {
        Serial.println("Invalid response");
        return;
    }

    if (json["config"].isNull())
    {
        Serial.println("Configuration not found");
        return;
    }

    JsonObject config = json["config"];

    if (config.isNull())
    {
        return;
    }

    storage.setConfigVersion(
        config["version"] | 1);

    storage.setHeartbeatInterval(
        config["heartbeat"]["heartbeatSeconds"] | 30);

    JsonArray sensors = json["sensors"];

    if (!sensors.isNull() && sensors.size() > 0)
    {
        JsonObject sensor = sensors[0];

        JsonObject sensorConfig = sensor["sensorConfig"];

        storage.setTemperatureEnabled(
            sensorConfig["enabled"] | true);

        storage.setTemperatureInterval(
            sensorConfig["interval"] | 60);
    } else {
        Serial.println("SENSORES NULL");
    }

    Serial.println();
    Serial.println("====== CONFIGURATION ======");

    Serial.print("Config version.......: ");
    Serial.println(storage.getConfigVersion());

    Serial.print("Heartbeat............: ");
    Serial.print(storage.getHeartbeatInterval());
    Serial.println(" s");

    Serial.print("Temperature enabled..: ");
    Serial.println(storage.isTemperatureEnabled() ? "true" : "false");

    Serial.print("Temperature interval.: ");
    Serial.print(storage.getTemperatureInterval());
    Serial.println(" s");

    Serial.println("===========================");
}