#pragma once

#include "../../models/SensorInfo.h"

class SensorService
{
    public:
        void init();
        void update();

        SensorInfo& sensor();

    private:

        SensorInfo sensorInfo;
    uint32_t seenVersion = 0xFFFFFFFFu;

};
