#pragma once

#include "SensorEntryInfo.h"
#include <stdint.h>

struct SensorInfo
{
    char id[37];

    char name[32];

    char unit[8];

    float value = 0.0f;

    bool valid = false;

    int total = 0;

    int online = 0;

    int offline = 0;

    SensorEntryInfo sensorsList[5];
    int sensorListCount = 0;
};