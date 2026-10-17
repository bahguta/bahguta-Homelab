#pragma once

#include "../../models/DeviceInfo.h"

class DeviceService
{
public:
    void init();
    void update();

    DeviceInfo& device();

private:
    DeviceInfo deviceInfo;
    uint32_t seenVersion = 0xFFFFFFFFu;
};