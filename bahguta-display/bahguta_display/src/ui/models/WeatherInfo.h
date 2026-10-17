#pragma once

#include <Arduino.h>

struct WeatherInfo
{
    bool success = false;

    String location;
    String country;

    float temperature = 0.0f;
    float feelsLike = 0.0f;
    float minTemperature = 0.0f;
    float maxTemperature = 0.0f;

    int humidity = 0;
    int pressure = 0;

    String description;
    String icon;

    float windSpeed = 0.0f;
    int windDirection = 0;

    int clouds = 0;

    const char* sunrise = 0;
    const char* sunset = 0;
    int timezone = 0;

    unsigned long updatedAt = 0;


    // ============================================================
    // FORECAST (una entrada representativa por dia)
    // ============================================================

    // Franjas del dia (hasta 3, ~08:00 / 14:00 / 20:00): hora local + icono de OpenWeather
    static constexpr int FORECAST_HOURS = 3;

    struct ForecastHour
    {
        uint8_t hour = 0;           // hora de referencia: 8, 14 o 20
        float minTemperature = 0.0f;   // minima / maxima de ese periodo
        float maxTemperature = 0.0f;
        char icon[4] = "";          // "10n"
    };

    struct ForecastEntry
    {
        char label[20] = "";     // "Miércoles\n20/09"
        char icon[4] = "";      // "10d"
        float temperature = 0.0f;   // = maxTemperature (compatibilidad)
        float minTemperature = 0.0f;
        float maxTemperature = 0.0f;

        ForecastHour hours[FORECAST_HOURS];
        uint8_t hourCount = 0;
    };

    static constexpr int FORECAST_MAX_DAYS = 6;

    ForecastEntry forecast[FORECAST_MAX_DAYS];
    uint8_t forecastCount = 0;

};