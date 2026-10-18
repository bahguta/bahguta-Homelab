#include "SensorService.h"
#include "../ServiceManager.h"

void SensorService::init()
{
}

void SensorService::update()
{
    const uint32_t v = ServiceManager::api().version();

    if (v == seenVersion)
        return;

    seenVersion = v;

    sensorInfo = ServiceManager::api().sensors();
}

SensorInfo& SensorService::sensor()
{
    return sensorInfo;
}