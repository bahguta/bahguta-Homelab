#include "DeviceService.h"
#include "../ServiceManager.h"

void DeviceService::init()
{
}

void DeviceService::update()
{
    const uint32_t v = ServiceManager::api().version();

    if (v == seenVersion)
        return;

    seenVersion = v;

    deviceInfo = ServiceManager::api().devices();
}

DeviceInfo& DeviceService::device()
{
    return deviceInfo;
}