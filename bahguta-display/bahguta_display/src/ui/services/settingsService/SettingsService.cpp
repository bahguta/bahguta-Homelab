#include "SettingsService.h"

#include <Preferences.h>

static Preferences preferences;

void SettingsService::init()
{
    load();
}

bool SettingsService::load()
{
    /*
     * IMPORTANTE: se abre en modo lectura/escritura (false),
     * no en solo-lectura.
     *
     * Preferences::begin(name, true) falla si el namespace
     * "bahguta" todavia no existe en NVS (no puede crearlo en
     * modo solo-lectura). Como nada en el firmware llama a
     * save(), ese namespace nunca se habia creado, load()
     * devolvia false y wifiSsid/wifiPassword se quedaban
     * siempre vacios sin importar el valor por defecto puesto
     * en getString().
     */

    if (!preferences.begin("bahguta", false))
    {
        return false;
    }

    currentSettings.configured =
        preferences.getBool(
            "configured",
            false);

    currentSettings.deviceName =
        preferences.getString(
            "deviceName",
            "bahguta_Display");

    currentSettings.wifiSsid =
        preferences.getString(
            "wifiSsid",
            "");

    currentSettings.wifiPassword =
        preferences.getString(
            "wifiPass",
            "");

    currentSettings.backendUrl =
        preferences.getString(
            "backend",
            "http://bahguta.local");

    currentSettings.timezone =
        preferences.getString(
            "timezone",
            "CET-1CEST,M3.5.0,M10.5.0/3");

    currentSettings.refreshInterval =
        preferences.getUInt(
            "refresh",
            30000);

    currentSettings.inactivitySeconds =
        preferences.getUInt(
            "inactivity",
            60);

    preferences.end();

    return true;
}

bool SettingsService::save()
{
    if (!preferences.begin("bahguta", false))
    {
        return false;
    }

    preferences.putBool(
        "configured",
        currentSettings.configured);

    preferences.putString(
        "deviceName",
        currentSettings.deviceName);

    preferences.putString(
        "wifiSsid",
        currentSettings.wifiSsid);

    preferences.putString(
        "wifiPass",
        currentSettings.wifiPassword);

    preferences.putString(
        "backend",
        currentSettings.backendUrl);

    preferences.putString(
        "timezone",
        currentSettings.timezone);

    preferences.putUInt(
        "refresh",
        currentSettings.refreshInterval);

    preferences.putUInt(
        "inactivity",
        currentSettings.inactivitySeconds);

    preferences.end();

    return true;
}

SettingsInfo& SettingsService::settings()
{
    return currentSettings;
}