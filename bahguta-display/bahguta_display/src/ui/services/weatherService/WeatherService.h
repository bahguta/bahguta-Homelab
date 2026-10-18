#pragma once

#include "../../models/WeatherInfo.h"

class WeatherService
{
public:
    void init();
    void update();

    const WeatherInfo& weather() const;

private:
    WeatherInfo currentWeather;
    uint32_t seenVersion = 0xFFFFFFFFu;
};