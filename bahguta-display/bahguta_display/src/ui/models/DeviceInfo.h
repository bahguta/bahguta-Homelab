#pragma once

#include <stdint.h>
#include <Arduino.h>
#include "DeviceEntryInfo.h"

struct DeviceInfo
{
    char id[37];

    char name[32];

    char room[32];

    //bool online = false;

    uint8_t battery = 0;

    int8_t wifi = 0;

    int total = 0;

    int online = 0;

    int offline = 0;

    DeviceEntryInfo devicesList[5];
    int deviceListCount = 0;
};