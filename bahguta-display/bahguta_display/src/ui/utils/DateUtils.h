#pragma once

#include "../models/ClockInfo.h"
#include <Arduino.h>

class DateUtils
{
public:

    static const char* weekday(uint8_t day);

    static uint8_t weekdayFromDate(int year, int month, int day);

    static const char* month(uint8_t month);

    static void formatTime(
        const ClockInfo& clock,
        char* buffer);

    static void formatDate(
        const ClockInfo& clock,
        char* buffer);
};