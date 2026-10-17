#pragma once

#include <Arduino.h>

struct SensorEntryInfo
{
    String id;
    String deviceId;
    String name;
    String type;

    bool enabled = false;
    int interval = 0;
};