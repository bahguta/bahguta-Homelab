#pragma GCC push_options
#pragma GCC optimize("O3")

#include <Arduino.h>
#include <LittleFS.h>
#include "lvgl.h"

#include "pins_config.h"
#include "src/lcd/jd9365_lcd.h"
#include "src/touch/gsl3680_touch.h"
#include "src/ui/layout/MainLayout.h"
#include "src/ui/services/ServiceManager.h"

// #define PIN_TACTIL_INT 22

SET_LOOP_TASK_STACK_SIZE(16 * 1024);  

static constexpr int BATTERY_PIN = 52;

jd9365_lcd lcd = jd9365_lcd(LCD_RST);
gsl3680_touch touch = gsl3680_touch(TP_I2C_SDA, TP_I2C_SCL, TP_RST, TP_INT);

lv_display_t * disp_drv;

static lv_color_t *buf;
static lv_color_t *buf1;

static uint32_t flushWaitMaxUs = 0;
static uint32_t flushWaitCount = 0;
static uint32_t flushWaitTimeouts = 0;
static uint32_t lastFlushStatsPrint = 0;

void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * color_map)
{
  const int offsetx1 = area->x1;
  const int offsetx2 = area->x2;
  const int offsety1 = area->y1;
  const int offsety2 = area->y2;
  lcd.lcd_draw_bitmap(offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);

  uint32_t waitStart = micros();
  lcd.waitFlushDone();
  uint32_t waitUs = micros() - waitStart;

  flushWaitCount++;
  if (waitUs > flushWaitMaxUs) flushWaitMaxUs = waitUs;
  if (waitUs >= 95000) flushWaitTimeouts++;

  if (millis() - lastFlushStatsPrint > 2000)
  {
      lastFlushStatsPrint = millis();
      flushWaitMaxUs = 0;
  }

  lv_display_flush_ready(disp);
}

void my_touchpad_read(
    lv_indev_t *indev_driver,
    lv_indev_data_t *data)
{
    bool touched;
    uint16_t touchX, touchY;

    touched =touch.getTouch(&touchX,&touchY);
    if (!touched)
    {
        data->state =LV_INDEV_STATE_REL;
         ServiceManager::power().touchReleased();
        return;
    }

    /*
     * IMPORTANTE: activity() se llama ANTES de comprobar
     * ignoreTouch(). Si la pantalla estaba apagada, activity()
     * la despierta y marca ignoreTouchUntilRelease = true en
     * esta misma llamada -- así el propio toque que despierta
     * la pantalla tambien queda ignorado, en vez de colarse
     * como un click real sobre lo que hubiera debajo.
     */

    ServiceManager::power().activity();

    if (ServiceManager::power().ignoreTouch())
    {
        data->state = LV_INDEV_STATE_REL;
        return;
    }

    data->state =LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;

}
    static uint32_t lvglTick() { return millis(); }

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.printf("Reset reason: %d\n", esp_reset_reason());

    if (!LittleFS.begin(true))
    {
        Serial.println("ERROR: LittleFS mount failed");
    }

    lcd.begin();
    touch.begin();

    lv_init();


    disp_drv = lv_display_create(LCD_H_RES, LCD_V_RES);

    if (!disp_drv)
    {
        Serial.println("ERROR: lv_display_create FAILED");
        while (true)
            delay(1000);
    }

    lv_display_set_flush_cb(disp_drv, my_disp_flush);

    /*
     * MODO PARTIAL en vez de FULL: antes se redibujaba y
     * mandaba la pantalla ENTERA (800x1280, ~2MB) por cada
     * cambio, aunque fuera un icono pequeño -- de ahi los
     * tirones, ya que esp_lcd_panel_draw_bitmap() bloquea la
     * CPU hasta terminar de mandar ese frame completo por
     * MIPI-DSI.
     *
     * Con PARTIAL, LVGL solo redibuja y manda la zona que
     * realmente cambio, usando un buffer bastante mas
     * pequeno (aqui, 200 filas en vez de las 1280 completas).
     */

    /*
     * Vuelta a FULL: el intento de PARTIAL (+ esperar la
     * confirmacion real de hardware) genero mas problemas de
     * los que resolvia -- primero rastro entre pantallas,
     * despues un cuelgue total del touch. Mejor volver al
     * ultimo estado confirmado estable y replantear el tema
     * de rendimiento con mas cuidado, cambiando una sola cosa
     * cada vez.
     */

    /*
     * PARTIAL, segundo intento, esta vez con la espera de
     * jd9365_lcd::waitFlushDone() acotada a 100ms (no
     * portMAX_DELAY) -- si el hardware no confirma, seguimos
     * de todas formas en vez de congelar el touch para siempre.
     * Buffer de 300 filas (antes 200), para necesitar menos
     * pasadas de flush por cambio de pantalla.
     */

    const size_t buffer_pixels =
        LCD_H_RES * 300;

    const size_t buffer_bytes =
        buffer_pixels * 2;

    Serial.printf("LVGL buffer pixels: %u\n", buffer_pixels);
    Serial.printf("LVGL buffer bytes: %u\n", buffer_bytes);
    Serial.printf("LVGL color size: %u\n", sizeof(lv_color_t));

    buf = (lv_color_t *)heap_caps_malloc(
        buffer_bytes,
        MALLOC_CAP_SPIRAM
    );

    buf1 = (lv_color_t *)heap_caps_malloc(
        buffer_bytes,
        MALLOC_CAP_SPIRAM
    );

    if (!buf || !buf1)
    {
        Serial.println("ERROR: LVGL buffer allocation FAILED");
        while (true)
            delay(1000);
    }

    Serial.printf("buf  = %p\n", buf);
    Serial.printf("buf1 = %p\n", buf1);

    lv_display_set_buffers(
        disp_drv,
        buf,
        buf1,
        buffer_bytes,
        LV_DISPLAY_RENDER_MODE_PARTIAL
    );

    lv_indev_t * indev = lv_indev_create();

    lv_indev_set_type(
        indev,
        LV_INDEV_TYPE_POINTER
    );

    lv_indev_set_read_cb(
        indev,
        my_touchpad_read
    );

    analogReadResolution(12);
    analogSetPinAttenuation(
        BATTERY_PIN,
        ADC_11db
    );


  ServiceManager::init();
  MainLayout::init();

  // Network/HTTP/backend work runs on the other CPU core.
  // LVGL remains exclusively on the Arduino loop task.
  ServiceManager::startRuntimeTask();
}

void loop()
{
  // Runtime services (WiFi/HTTP/backend) run on the dedicated
  // FreeRTOS task. The Arduino loop is reserved for LVGL/UI.

  ServiceManager::lockData();

  MainLayout::update();

  ServiceManager::unlockData();

  ServiceManager::updateUi();

  lv_timer_handler();
  lv_tick_set_cb(lvglTick);
  delay(5);
}