#pragma once

#include <Arduino.h>

class PowerService
{
public:

    void init();
    void update();

    void activity();

    bool ignoreTouch() const;
    void touchReleased();

    void shutdown();

    // Tiempo sin tocar la tablet antes de apagar la pantalla. 0 = nunca.
    // Se guarda en la tablet (NVS), no en el backend.
    void setInactivityTimeoutSeconds(uint32_t seconds);
    uint32_t inactivityTimeoutSeconds() const;

private:

    void sleepDisplay();
    void wakeDisplay();
    void configureWakeup();

    void setBacklight(uint8_t level);
    void startFade(uint8_t to, uint32_t ms);
    void updateFade();

    uint32_t lastActivity = 0;

    bool sleeping = false;

    // Fade de backlight
    uint8_t  backlightLevel = 255;
    uint8_t  fadeFrom = 255;
    uint8_t  fadeTo = 255;
    uint32_t fadeStart = 0;
    uint32_t fadeDuration = 0;
    bool     fading = false;
    bool     blankPending = false;

    static constexpr uint32_t FADE_OUT_MS = 800;
    static constexpr uint32_t FADE_IN_MS = 500;
    static constexpr uint32_t BACKLIGHT_PWM_HZ = 20000;
    
    bool ignoreTouchUntilRelease = false;

    // Valor por defecto si no hay nada guardado; el real sale de la NVS (0 = nunca)
    uint32_t inactivityTimeoutMs = 1UL * 60UL * 1000UL;
    static constexpr uint32_t SCHEDULED_REBOOT_INTERVAL = 3UL * 60UL * 60UL * 1000UL;

};