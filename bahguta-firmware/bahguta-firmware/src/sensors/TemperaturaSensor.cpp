#include "TemperatureSensor.h"

#include <Arduino.h>

void TemperatureSensor::setup()
{
    dht.begin();
}

void TemperatureSensor::loop()
{
    if (!storage.isTemperatureEnabled())
    {
        return;
    }

    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity))
    {
        Serial.println("Error leyendo DHT22");
        return;
    }


    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" ºC");

    Serial.print("   Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.println("===========================");
}


Measurement TemperatureSensor::getMeasurement()
{
    Measurement measurement;

    measurement.temperature = temperature;
    measurement.humidity = humidity;

    return measurement;
}