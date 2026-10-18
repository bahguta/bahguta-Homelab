#pragma once

#include <Arduino.h>
#include <Preferences.h>

class Storage
{
public:

    String getDeviceUuid();

    String getSensorUuid(const String& sensorName);

    long getConfigVersion();
    void setConfigVersion(long version);

    int getHeartbeatInterval();
    void setHeartbeatInterval(int seconds);

    bool isTemperatureEnabled();
    void setTemperatureEnabled(bool enabled);

    int getTemperatureInterval();
    void setTemperatureInterval(int seconds);

    bool isRegistered();
    void setRegistered(bool registered);

private:

    String getOrCreate(const String& key);

    String generateUuid();

    Preferences preferences;
};

