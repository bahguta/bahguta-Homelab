#pragma once

#include <WiFi.h>

class WifiManager
{
public:

    void setup();

    bool isConnected() const;

    String getIpAddress() const;
};
