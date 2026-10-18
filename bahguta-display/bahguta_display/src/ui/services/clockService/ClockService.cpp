#include "ClockService.h"
#include <Arduino.h>
#include <time.h>
#include "../ServiceManager.h"
#include "../../models/ClockInfo.h"

void ClockService::init()
{
    configTime(
        0,
        0,
        "pool.ntp.org",
        "time.nist.gov");

    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
}

void ClockService::update()
{
    if(!ServiceManager::network().network().connected)
    {
        currentClock.valid = false;
        return;
    }

    time_t now;
    time(&now);

    if(now < 1700000000)
    {
        currentClock.valid = false;
        return;
    }

    struct tm t;

    localtime_r(&now, &t);

    if(previousSecond == t.tm_sec)
        return;

    previousSecond = t.tm_sec;

    currentClock.year = t.tm_year + 1900;
    currentClock.month = t.tm_mon + 1;
    currentClock.day = t.tm_mday;

    currentClock.hour = t.tm_hour;
    currentClock.minute = t.tm_min;
    currentClock.second = t.tm_sec;

    currentClock.weekday = t.tm_wday;

    currentClock.valid = true;
}

const ClockInfo& ClockService::clock() const
{
    return currentClock;
}