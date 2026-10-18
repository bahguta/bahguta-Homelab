#include "WeatherService.h"

#include "../ServiceManager.h"

void WeatherService::init()
{
}

void WeatherService::update()
{
    const uint32_t v = ServiceManager::api().version();

    if (v == seenVersion)
        return;

    seenVersion = v;

    currentWeather = ServiceManager::api().weather();
}

const WeatherInfo& WeatherService::weather() const
{
    return currentWeather;
}