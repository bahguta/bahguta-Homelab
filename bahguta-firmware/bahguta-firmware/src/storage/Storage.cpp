#include "Storage.h"

#include <esp_system.h>

static const char* KEY_CONFIG_VERSION = "cfgVer";
static const char* KEY_HEARTBEAT = "hb";
static const char* KEY_TEMP_ENABLED = "tEn";
static const char* KEY_TEMP_INTERVAL = "tInt";
static const char* KEY_REGISTERED = "registered";

String Storage::getDeviceUuid()
{
    return getOrCreate("device");
}

String Storage::getSensorUuid(const String& sensorName)
{
    return getOrCreate(sensorName);
}

String Storage::getOrCreate(const String& key)
{
    preferences.begin("bahguta", false);

    String value = preferences.getString(key.c_str(), "");

    if (value.isEmpty())
    {
        value = generateUuid();

        preferences.putString(key.c_str(), value);
    }

    preferences.end();

    return value;
}

String Storage::generateUuid()
{
    uint8_t bytes[16];

    for (int i = 0; i < 16; i++)
    {
        bytes[i] = (uint8_t)esp_random();
    }

    // UUID v4
    bytes[6] = (bytes[6] & 0x0F) | 0x40;
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    char uuid[37];

    snprintf(
        uuid,
        sizeof(uuid),
        "%02x%02x%02x%02x-"
        "%02x%02x-"
        "%02x%02x-"
        "%02x%02x-"
        "%02x%02x%02x%02x%02x%02x",
        bytes[0], bytes[1], bytes[2], bytes[3],
        bytes[4], bytes[5],
        bytes[6], bytes[7],
        bytes[8], bytes[9],
        bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);

    return String(uuid);
}

long Storage::getConfigVersion()
{
    preferences.begin("bahguta", false);

    long version = preferences.getLong(KEY_CONFIG_VERSION, 1);

    preferences.end();

    return version;
}

void Storage::setConfigVersion(long version)
{
    preferences.begin("bahguta", false);

    preferences.putLong(KEY_CONFIG_VERSION, version);

    preferences.end();
}

int Storage::getHeartbeatInterval()
{
    preferences.begin("bahguta", false);

    int seconds = preferences.getInt(KEY_HEARTBEAT, 30);

    preferences.end();

    return seconds;
}

void Storage::setHeartbeatInterval(int seconds)
{
    preferences.begin("bahguta", false);

    preferences.putInt(KEY_HEARTBEAT, seconds);

    preferences.end();
}

bool Storage::isTemperatureEnabled()
{
    preferences.begin("bahguta", false);

    bool enabled = preferences.getBool(KEY_TEMP_ENABLED, true);

    preferences.end();

    return enabled;
}

void Storage::setTemperatureEnabled(bool enabled)
{
    preferences.begin("bahguta", false);

    preferences.putBool(KEY_TEMP_ENABLED, enabled);

    preferences.end();
}

int Storage::getTemperatureInterval()
{
    preferences.begin("bahguta", false);

    int interval = preferences.getInt(KEY_TEMP_INTERVAL, 60);

    preferences.end();

    return interval;
}

void Storage::setTemperatureInterval(int seconds)
{
    preferences.begin("bahguta", false);

    preferences.putInt(KEY_TEMP_INTERVAL, seconds);

    preferences.end();
}

bool Storage::isRegistered()
{
    preferences.begin("bahguta", false);

    bool registered =
        preferences.getBool(KEY_REGISTERED, false);

    preferences.end();

    return registered;
}

void Storage::setRegistered(bool registered)
{
    preferences.begin("bahguta", false);

    preferences.putBool(KEY_REGISTERED, registered);

    preferences.end();
}