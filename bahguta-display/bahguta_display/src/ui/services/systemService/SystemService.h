#pragma once

#include "../../models/SystemInfo.h"

class SystemService
{
public:

    void init();
    void update();

    SystemInfo& system();

private:

    SystemInfo systemInfo;

    static constexpr int BATTERY_PIN = 52;

    static constexpr int LOW_BATTERY_SHUTDOWN_PERCENT = 15;

    /*
     * Si el pin lee menos que esto, asumimos que NO hay bateria
     * conectada (tablet corriendo solo por USB) en vez de
     * "bateria a 0%" -- sin esto, el apagado preventivo se
     * disparaba nada mas arrancar sin bateria, en bucle.
     */
    static constexpr float BATTERY_NOT_PRESENT_VOLTAGE = 1.0f;

    int lowBatteryCount = 0;

    static constexpr float BATTERY_MIN_VOLTAGE = 1.80f;
    static constexpr float BATTERY_MAX_VOLTAGE = 2.43f;

    double batteryVoltage = 0.0;
    double batteryFiltered = 0.0;
    int lastReportedBattery = -1;

    // ========================================================
    // CPU MONITORING
    // ========================================================

    // uint64_t lastCpuRuntime = 0;
    // uint64_t lastCpuTimestamp = 0;

    // double calculateCpuUsage();

    // ========================================================
    // BATTERY
    // ========================================================

    int calculateBatteryPercentage(float voltage);
};