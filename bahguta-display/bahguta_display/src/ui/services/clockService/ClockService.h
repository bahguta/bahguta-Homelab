#pragma once

#include <time.h>
#include "../../models/ClockInfo.h"
#include <Arduino.h>

class ClockService
{
public:

    void init();

    void update();

    const ClockInfo& clock() const;

private:

    void refresh();

    ClockInfo currentClock;

    bool synced = false;

    time_t lastSync = 0;

    uint8_t previousSecond = 255;

    static bool isTimeValid();
};