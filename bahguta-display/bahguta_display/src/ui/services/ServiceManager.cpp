#include "ServiceManager.h"
#include <Arduino.h>

ClockService ServiceManager::clockService;
WeatherService ServiceManager::weatherService;
NetworkService ServiceManager::networkService;
ApiService ServiceManager::apiService;
HomeService ServiceManager::homeService;
SystemService ServiceManager::systemService;
DeviceService ServiceManager::deviceService;
SensorService ServiceManager::sensorService;
DockerService ServiceManager::dockerService;
SettingsService ServiceManager::settingsService;
PowerService ServiceManager::powerService;

SemaphoreHandle_t ServiceManager::dataMutex = nullptr;
TaskHandle_t ServiceManager::runtimeTaskHandle = nullptr;

void ServiceManager::init()
{
    if (dataMutex == nullptr)
        dataMutex = xSemaphoreCreateMutex();

    if (dataMutex == nullptr)
    {
        Serial.println("[RUNTIME] ERROR: data mutex creation failed");
    }

    settingsService.init();
    networkService.init();
    apiService.init();
    systemService.init();
    clockService.init();
    weatherService.init();
    homeService.init();
    sensorService.init();
    deviceService.init();
    dockerService.init();
    powerService.init();
}

void ServiceManager::startRuntimeTask()
{
    if (runtimeTaskHandle != nullptr)
        return;

    runtimeTaskHandle = nullptr;

    BaseType_t result = xTaskCreatePinnedToCore(
        runtimeTask,
        "bahguta_runtime",
        12288,
        nullptr,
        1,
        &runtimeTaskHandle,
        0
    );

    if (result != pdPASS)
    {
        runtimeTaskHandle = nullptr;
        Serial.println("[RUNTIME] ERROR: task creation failed");
        return;
    }

    Serial.println("[RUNTIME] task created (pinned to core 0)");
}

void ServiceManager::runtimeTask(void* parameter)
{
    (void)parameter;

    Serial.printf(
        "[RUNTIME] running on core %d\n",
        xPortGetCoreID()
    );

    for (;;)
    {
        // Keep the mutex only around fast in-memory operations.
        // HTTP requests are deliberately executed WITHOUT this mutex,
        // so the LVGL task can never wait for a network timeout.

        lockData();
        networkService.update();
        systemService.update();
        clockService.update();
        unlockData();

        // ApiService performs HTTP requests and locks only while
        // publishing the resulting data.
        apiService.update();

        lockData();
        weatherService.update();
        homeService.update();
        sensorService.update();
        deviceService.update();
        dockerService.update();
        unlockData();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void ServiceManager::updateUi()
{
    // Power management touches the LCD and may enter light sleep.
    // It must remain on the same task as LVGL.
    powerService.update();
}

void ServiceManager::lockData()
{
    if (dataMutex != nullptr)
        xSemaphoreTake(dataMutex, portMAX_DELAY);
}

void ServiceManager::unlockData()
{
    if (dataMutex != nullptr)
        xSemaphoreGive(dataMutex);
}

HomeService& ServiceManager::home()
{
    return homeService;
}

ApiService& ServiceManager::api()
{
    return apiService;
}

NetworkService& ServiceManager::network()
{
    return networkService;
}

ClockService& ServiceManager::clock()
{
    return clockService;
}

WeatherService& ServiceManager::weather()
{
    return weatherService;
}

SystemService& ServiceManager::system()
{
    return systemService;
}

DeviceService& ServiceManager::device()
{
    return deviceService;
}

SensorService& ServiceManager::sensor()
{
    return sensorService;
}

DockerService& ServiceManager::docker()
{
    return dockerService;
}

SettingsService& ServiceManager::settings()
{
    return settingsService;
}

PowerService& ServiceManager::power()
{
    return powerService;
}
