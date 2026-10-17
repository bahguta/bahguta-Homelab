#include "PowerService.h"

#include <Arduino.h>
#include <esp_sleep.h>
#include <lvgl.h>

#include "../../../../pins_config.h"

#include "../../../lcd/jd9365_lcd.h"
#include "../ServiceManager.h"

extern jd9365_lcd lcd;


void PowerService::init()
{
    lastActivity = millis();

    // Tiempo de inactividad guardado en la tablet (0 = no apagar nunca la pantalla)
    inactivityTimeoutMs =
        ServiceManager::settings().settings().inactivitySeconds * 1000UL;

    // LCD backlight por PWM (permite fade in / fade out)
    ledcAttach(LCD_LED, BACKLIGHT_PWM_HZ, 8);
    setBacklight(255);

    // Touch interrupt
    pinMode(TP_INT, INPUT_PULLUP);

    Serial.println();
    Serial.println("================================");
    Serial.println("       BAHGUTA POWER SERVICE");
    Serial.println("================================");
    Serial.println("Display auto-blank enabled (CPU stays awake).");
    Serial.println("Touch wakeup: polling (instant).");
    Serial.println("LCD backlight GPIO: 23");
    if (inactivityTimeoutMs == 0)
        Serial.println("Timeout: never (display always on)");
    else
        Serial.printf("Timeout: %lu s\n", (unsigned long)(inactivityTimeoutMs / 1000UL));
    Serial.println("================================");
}


void PowerService::update()
{
    updateFade();

    if (sleeping)
    {
        if (millis() >= SCHEDULED_REBOOT_INTERVAL)
        {
            Serial.println("Reinicio preventivo (muchas horas seguidas encendida, pantalla ya dormida).");
            ESP.restart();
        }

        return;
    }

    // 0 = nunca apagar la pantalla
    if (inactivityTimeoutMs != 0 &&
        millis() - lastActivity >= inactivityTimeoutMs)
    {
        sleepDisplay();
    }
}


void PowerService::setInactivityTimeoutSeconds(uint32_t seconds)
{
    // Menos de 10 s no tiene sentido (la pantalla se apagaria mientras se usa)
    if (seconds != 0 && seconds < 10)
        seconds = 10;

    inactivityTimeoutMs = seconds * 1000UL;

    // Se acaba de tocar la pantalla para cambiarlo: la cuenta empieza ahora
    lastActivity = millis();

    // Persistir solo en la tablet
    ServiceManager::settings().settings().inactivitySeconds = seconds;
    ServiceManager::settings().save();

    Serial.printf("Screen timeout -> %lu s (NVS)\n", (unsigned long)seconds);
}


uint32_t PowerService::inactivityTimeoutSeconds() const
{
    return inactivityTimeoutMs / 1000UL;
}


void PowerService::activity()
{
    lastActivity = millis();

    if (sleeping)
    {
        sleeping = false;
        wakeDisplay();
    }
}


void PowerService::configureWakeup()
{
    Serial.println("Configuring touch wakeup...");

    /*
     * GSL3680 INT:
     *
     * HIGH -> idle
     * LOW  -> touch
     */

    esp_sleep_enable_gpio_wakeup();

    gpio_wakeup_enable(
        (gpio_num_t)TP_INT,
        GPIO_INTR_LOW_LEVEL
    );
}


/*
 * "Sleep" de pantalla, NO de CPU.
 *
 * Se probo con esp_light_sleep_start() y el panel MIPI-DSI
 * no lo soportaba de forma fiable (underrun al despertar,
 * disp_sleep() no soportado por el driver -- confirmado en
 * logs reales). En vez de perseguir eso, apagamos solo lo
 * visual: backlight y salida de video. La CPU sigue a pleno
 * rendimiento, el touch se sigue leyendo por sondeo normal
 * (ya funcionaba), y el "despertar" es instantaneo porque no
 * hay ninguna transicion de hardware real que gestionar.
 *
 * Para una tablet enchufada a la pared, el ahorro de energia
 * de verdad esta en apagar la retroiluminacion (que es lo
 * que mas consume), no en dormir la CPU.
 */

void PowerService::sleepDisplay()
{
    sleeping = true;

    Serial.println();
    Serial.println("================================");
    Serial.println("       POWER MANAGEMENT");
    Serial.println("================================");
    Serial.println("Inactivity timeout reached.");
    Serial.println("Blanking display (CPU stays awake).");
    Serial.println("================================");

    // Fade out; el panel se apaga (lcd.sleep) al terminar el fade (ver updateFade)
    blankPending = true;
    startFade(0, FADE_OUT_MS);
}


void PowerService::wakeDisplay()
{
    Serial.println();
    Serial.println("================================");
    Serial.println("           WAKE UP");
    Serial.println("================================");

    ignoreTouchUntilRelease = true;

    if (blankPending)
    {
        // El fade-out aun no habia terminado: el panel sigue encendido,
        // basta con volver a subir el brillo desde donde estaba.
        blankPending = false;
        startFade(255, FADE_IN_MS);
        return;
    }

    lcd.wake();

    delay(120);

    // Primer frame con el backlight a 0 y luego fade in
    setBacklight(0);
    lv_obj_invalidate(lv_screen_active());
    startFade(255, FADE_IN_MS);

    Serial.println("LCD backlight fade in.");
    Serial.println("================================");
}


// Brillo lineal 0..255 con correccion gamma (el ojo percibe mejor el fade)
void PowerService::setBacklight(uint8_t level)
{
    backlightLevel = level;
    uint32_t duty = ((uint32_t)level * level) / 255;
    ledcWrite(LCD_LED, duty);
}


void PowerService::startFade(uint8_t to, uint32_t ms)
{
    fadeFrom     = backlightLevel;
    fadeTo       = to;
    fadeStart    = millis();
    fadeDuration = ms;
    fading       = true;
}


void PowerService::updateFade()
{
    if (!fading) return;

    uint32_t t = millis() - fadeStart;

    if (t >= fadeDuration)
    {
        setBacklight(fadeTo);
        fading = false;

        if (blankPending && fadeTo == 0)
        {
            blankPending = false;
            lcd.sleep();
            Serial.println("Display blanked.");
        }
        return;
    }

    int32_t lvl = (int32_t)fadeFrom +
                  ((int32_t)fadeTo - (int32_t)fadeFrom) * (int32_t)t / (int32_t)fadeDuration;

    setBacklight((uint8_t)lvl);
}


bool PowerService::ignoreTouch() const
{
    return ignoreTouchUntilRelease;
}


void PowerService::touchReleased()
{
    if (ignoreTouchUntilRelease)
    {
        ignoreTouchUntilRelease = false;

        Serial.println(
            "Touch released -> touch input enabled."
        );
    }
}

void PowerService::shutdown()
{
    Serial.println();
    Serial.println("================================");
    Serial.println("     POWER OFF (DEEP SLEEP)");
    Serial.println("================================");
    Serial.println("Touch GPIO21 -> WAKE (reinicio)");
    Serial.println("================================");

    configureWakeup();

    Serial.println("Putting LCD panel into sleep...");

    lcd.sleep();

    Serial.println("Turning LCD backlight OFF...");

    setBacklight(0);

    /*
     * A diferencia del light sleep, el deep sleep no
     * retorna: el ESP32 se reinicia por completo al
     * despertar (vuelve a ejecutar setup()).
     */

    Serial.println("Entering DEEP SLEEP...");

    esp_deep_sleep_start();
}
