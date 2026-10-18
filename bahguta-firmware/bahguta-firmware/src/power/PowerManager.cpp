#include "PowerManager.h"

#include <WiFi.h>
#include <esp_sleep.h>

void PowerManager::sleep(unsigned long seconds)
{
    Serial.println();
    Serial.println("======== Deep Sleep =======");
    Serial.println("Entering Deep Sleep ...");
    Serial.print("Sleep: ");
    Serial.print(seconds);
    Serial.println(" seconds");
    Serial.println("===========================");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    esp_sleep_enable_timer_wakeup(
        (uint64_t)seconds * 1000000ULL);

    delay(200);

    esp_deep_sleep_start();
}