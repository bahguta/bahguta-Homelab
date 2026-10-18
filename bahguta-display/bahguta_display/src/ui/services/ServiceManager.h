#pragma once

#include "clockService/ClockService.h"
#include "networkService/NetworkService.h"
#include "weatherService/WeatherService.h"
#include "homeService/HomeService.h"
#include "apiService/ApiService.h"
#include "../models/ClockInfo.h"
#include "systemService/SystemService.h"
#include "deviceService/DeviceService.h"
#include "sensorService/SensorService.h"
#include "dockerService/DockerService.h"
#include "settingsService/SettingsService.h"
#include "powerService/PowerService.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"



class ServiceManager
{
public:

    static void init();

    // Runtime/data task. Runs independently from the LVGL loop.
    static void startRuntimeTask();

    // Called only from the LVGL/Arduino loop.
    static void updateUi();

    // Protects service data shared between the runtime task and UI.
    static void lockData();
    static void unlockData();

    static ClockService& clock();
    static NetworkService& network();
    static WeatherService& weather();
    static HomeService& home();
    static ApiService& api();
    static SystemService& system();
    static DeviceService& device();
    static SensorService& sensor();
    static DockerService& docker();
    static SettingsService& settings();
    static PowerService& power();

private:

    static ClockService clockService;
    static NetworkService networkService;
    static WeatherService weatherService;
    static HomeService homeService;
    static ApiService apiService;
    static SystemService systemService;
    static DeviceService deviceService;
    static SensorService sensorService;
    static DockerService dockerService;
    static SettingsService settingsService;
    static PowerService powerService;

    static SemaphoreHandle_t dataMutex;
    static TaskHandle_t runtimeTaskHandle;

    static void runtimeTask(void* parameter);

};