#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_gsl3680.h"
#include "gsl3680_touch.h"
#include "esp_heap_caps.h"
#include <Arduino.h>

extern "C" {
    esp_err_t esp_lcd_touch_gsl3680_read_signature(esp_lcd_touch_handle_t tp, uint32_t *sig);
    esp_err_t esp_lcd_touch_gsl3680_recover(esp_lcd_touch_handle_t tp);
    extern volatile uint32_t gsl3680_i2c_errors;
    extern volatile uint32_t gsl3680_i2c_consecutive;
    extern volatile esp_err_t gsl3680_last_err;
    extern volatile uint32_t gsl3680_raw_finger_frames;
    extern volatile uint8_t gsl3680_raw[24];
    void esp_lcd_touch_gsl3680_reset_algorithm(void);
}

#define CONFIG_LCD_HRES 800
#define CONFIG_LCD_VRES 1280

static const char *TAG = "example";

esp_lcd_touch_handle_t tp;
esp_lcd_panel_io_handle_t tp_io_handle;

uint16_t touch_strength[1];
uint8_t touch_cnt = 0;

gsl3680_touch::gsl3680_touch(int8_t sda_pin, int8_t scl_pin, int8_t rst_pin, int8_t int_pin)
{
    _sda = sda_pin;
    _scl = scl_pin;
    _rst = rst_pin;
    _int = int_pin;
}

void gsl3680_touch::begin()
{
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = (gpio_num_t)_sda,
        .scl_io_num = (gpio_num_t)_scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
    };
    i2c_conf.master.clk_speed = 400000; // 400kHz

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, i2c_conf.mode, 0, 0, 0));

    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GSL3680_CONFIG();
    ESP_LOGI(TAG, "Initialize touch IO (I2C)");
    esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_NUM_0, &tp_io_config, &tp_io_handle);

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = CONFIG_LCD_HRES,
        .y_max = CONFIG_LCD_VRES,
        .rst_gpio_num = (gpio_num_t)_rst,
        .int_gpio_num = (gpio_num_t)_int,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 1,
        },
    };

    ESP_LOGI(TAG, "Initialize touch controller gsl3680");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_gsl3680(tp_io_handle, &tp_cfg, &tp));

    // Comprobacion visible del arranque: 0x5A5A5A5A = firmware del chip cargado
    uint32_t sig = 0;
    const esp_err_t e = esp_lcd_touch_gsl3680_read_signature(tp, &sig);
    Serial.printf("[TOUCH] arranque del controlador: err=%d firma=0x%08lX (%s)\n",
                  (int)e, (unsigned long)sig,
                  (e == ESP_OK && sig == 0x5a5a5a5aUL) ? "firmware OK" : "FIRMWARE NO VERIFICADO");
}

/*
 * Comandos por el Monitor Serie (para probar sin reflashear):
 *   r = reiniciar el controlador completo (reset + carga de firmware)
 *   a = reiniciar solo el algoritmo de software del driver
 */
void gsl3680_touch::handleSerialCommands()
{
    while (Serial.available() > 0)
    {
        const int c = Serial.read();

        if (c == 'r')
        {
            Serial.println("[TOUCH] comando 'r': reiniciando controlador...");
            const esp_err_t r = esp_lcd_touch_gsl3680_recover(tp);
            Serial.printf("[TOUCH] recuperacion -> %d (0 = firmware cargado)\n", (int)r);
        }
        else if (c == 'a')
        {
            Serial.println("[TOUCH] comando 'a': reiniciando algoritmo...");
            esp_lcd_touch_gsl3680_reset_algorithm();
        }
    }
}

bool gsl3680_touch::getTouch(uint16_t *x, uint16_t *y)
{
    handleSerialCommands();

    esp_lcd_touch_read_data(tp);
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(tp, x, y, touch_strength, &touch_cnt, 1);

    readCount++;

    if (touchpad_pressed)
        downCount++;
    else
        healthCheck();      // solo con el dedo fuera: no interfiere con un toque

    return touchpad_pressed;
}

/*
 * Vigilancia del controlador GSL3680.
 *
 * Cada 5 s lee su registro 0xb0 (firma 0x5a5a5a5a del firmware cargado).
 * La primera firma que se repite dos veces seguidas se toma como "buena".
 * Si despues falla la lectura o cambia la firma 2 veces seguidas, el
 * controlador se ha colgado o ha perdido su firmware (sintoma: "el touch
 * no responde" aunque el resto vaya bien): se repite su arranque.
 * Maximo 5 recuperaciones por arranque; cada una congela la UI ~1-2 s.
 *
 * Cada 10 s imprime una linea [TOUCH] con el estado, aunque todo vaya bien.
 */
void gsl3680_touch::healthCheck()
{
    const uint32_t now = millis();

    if (now - lastReportMs >= 10000)
    {
        lastReportMs = now;
    }

    if (now - lastHealthMs < 5000)
        return;

    lastHealthMs = now;

    uint32_t sig = 0;
    const esp_err_t e = esp_lcd_touch_gsl3680_read_signature(tp, &sig);

    lastSig = sig;
    lastSigErr = (int)e;

    // ---- calibracion: la misma firma valida dos veces seguidas ----
    if (!haveSignature)
    {
        if (e == ESP_OK && sig != 0 && sig != 0xFFFFFFFFu)
        {
            if (sig == candidateSignature)
            {
                goodSignature = sig;
                haveSignature = true;
                Serial.printf("[TOUCH] firma del controlador calibrada: 0x%08lX\n", (unsigned long)sig);
            }
            else
            {
                candidateSignature = sig;
            }
        }
        return;
    }

    // ---- vigilancia ----
    if (e == ESP_OK && sig == goodSignature)
    {
        badChecks = 0;
        return;
    }

    badChecks++;
    Serial.printf(
        "[TOUCH] vigilancia FALLO #%u: err=%d firma=0x%08lX (esperada 0x%08lX) i2cErr=%lu\n",
        (unsigned)badChecks, (int)e, (unsigned long)sig, (unsigned long)goodSignature,
        (unsigned long)gsl3680_i2c_errors);

    if (badChecks >= 2 && recoveries < 5)
    {
        recoveries++;
        Serial.printf("[TOUCH] controlador sin respuesta: reiniciandolo (intento %u/5)...\n", (unsigned)recoveries);

        const esp_err_t r = esp_lcd_touch_gsl3680_recover(tp);

        Serial.printf("[TOUCH] recuperacion terminada -> %d (0 = firmware cargado)\n", (int)r);

        badChecks = 0;
        gsl3680_i2c_consecutive = 0;
    }
}

void gsl3680_touch::set_rotation(uint8_t r){
switch(r){
    case 0:
        esp_lcd_touch_set_swap_xy(tp, false);   
        esp_lcd_touch_set_mirror_x(tp, false);
        esp_lcd_touch_set_mirror_y(tp, false);
        break;
    case 1:
        esp_lcd_touch_set_swap_xy(tp, false);
        esp_lcd_touch_set_mirror_x(tp, true);
        esp_lcd_touch_set_mirror_y(tp, true);
        break;
    case 2:
        esp_lcd_touch_set_swap_xy(tp, false);   
        esp_lcd_touch_set_mirror_x(tp, false);
        esp_lcd_touch_set_mirror_y(tp, false);
        break;
    case 3:
        esp_lcd_touch_set_swap_xy(tp, false);   
        esp_lcd_touch_set_mirror_x(tp, true);
        esp_lcd_touch_set_mirror_y(tp, true);
        break;
    }

}