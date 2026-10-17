#pragma once

#include <Arduino.h>

struct NetworkInfo
{
    bool connected = false;

    String ssid;

    String ip;

    int rssi = 0;
};