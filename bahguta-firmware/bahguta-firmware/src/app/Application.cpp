#include <Arduino.h>
#include "Application.h"

void Application::setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("===========================");
    Serial.println(" bahguta Firmware");
    Serial.println(" Developed by Plamen");
    Serial.println(" Version 1.0.0");
    Serial.println("===========================");

    wifiManager.setup();
    temperatureSensor.setup();
    apiClient.setTemperatureSensor(&temperatureSensor);

    if (!storage.isRegistered()) {
        Serial.println("First boot -> Register");
        apiClient.registerDevice();
    } else {
        Serial.println("Registered -> Heartbeat");
        apiClient.heartbeat();
    }

    temperatureSensor.loop();
    apiClient.registerMeasurement(temperatureSensor.getMeasurement());
    powerManager.sleep(storage.getHeartbeatInterval());
}