#pragma once

#include <Arduino.h>

struct RoomInfo
{
    String id;
    String name;

    double temperature = 0.0;
    double humidity = 0.0;

    bool temperatureAvailable = false;
    bool humidityAvailable = false;

    // true si la ultima medicion es antigua (sensor sin reportar):
    // el valor se muestra igualmente, atenuado.
    bool stale = false;

    String lastMeasurement;
};

struct CasaInfo
{
    bool success = false;

    String name = "Casa";

    // -------------------------
    // Temperatura general
    // -------------------------

    double averageTemperature = 0.0;
    bool temperatureAvailable = false;

    // Humedad media de las habitaciones que la reportan (%)
    double averageHumidity = 0.0;
    bool humidityAvailable = false;



    // -------------------------
    // Habitaciones
    // -------------------------

    RoomInfo rooms[12];

    uint8_t roomCount = 0;


    // -------------------------
    // Estado
    // -------------------------

    uint32_t updatedAt = 0;
};