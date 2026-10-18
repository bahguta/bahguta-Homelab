#pragma once

#include <Arduino.h>
#include <math.h>

/*
 * Historico de mediciones por habitacion para la grafica de CasaScreen.
 * Viene del backend ya agrupado (GET /api/v1/measurements/summary): una media por tramo.
 */

struct HistoryPoint
{
    uint32_t ts = 0;      // inicio del tramo (epoch, segundos)
    float t = NAN;        // temperatura media (C); NAN = sin dato
    float h = NAN;        // humedad media (%); NAN = sin dato
};

struct HistorySeries
{
    static constexpr uint8_t MAX_POINTS = 60;

    char room[28] = "";   // ubicacion del device, igual que el nombre de RoomInfo
    uint8_t count = 0;
    HistoryPoint points[MAX_POINTS];
};

struct CasaHistory
{
    static constexpr uint8_t MAX_SERIES = 6;

    bool available = false;        // se recibio algun histórico del backend
    uint32_t from = 0;             // ventana: epoch (s)
    uint32_t to = 0;
    uint32_t stepSeconds = 3600;   // duracion de cada tramo
    uint8_t seriesCount = 0;
    uint32_t version = 0;          // sube con cada actualizacion (para repintar solo si cambia)
    HistorySeries series[MAX_SERIES];
};
