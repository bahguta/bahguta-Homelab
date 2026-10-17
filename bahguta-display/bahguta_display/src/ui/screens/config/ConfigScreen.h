#pragma once

#include "../Screen.h"
#include <lvgl.h>

#include <stdint.h>


/*
 * CONFIG: configuracion actual (texto) y, debajo, los botones para cambiar
 * los intervalos de sensores, tablet y tiempo.
 */
class ConfigScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void show() override;
    void hide() override;
    void destroy() override;

private:

    enum Row : uint8_t
    {
        ROW_SENSORS = 0,   // backend: apiRefreshSeconds
        ROW_POLL    = 1,   // local (NVS): consulta tablet -> backend
        ROW_WEATHER = 2,   // backend: weatherRefreshSeconds
        ROW_SCREEN  = 3,   // local (NVS): segundos sin tocar antes de apagar la pantalla
        ROW_HOUR1   = 4,   // backend: hora (0-23) de la 1a franja del pronostico
        ROW_HOUR2   = 5,   // backend: hora de la 2a franja
        ROW_HOUR3   = 6,   // backend: hora de la 3a franja
        ROW_COUNT   = 7
    };

    enum Info : uint8_t
    {
        INFO_SENSORS = 0,
        INFO_POLL,
        INFO_WEATHER,
        INFO_SYSTEM,
        INFO_MQTT,
        INFO_LOCATION,
        INFO_BRIGHTNESS,
        INFO_SCREEN_TIMEOUT,     // local
        INFO_COUNT
    };

    struct ButtonCtx
    {
        ConfigScreen* self;
        uint8_t row;
        int8_t dir;        // -1 = menos, +1 = mas
    };

    void createContent();
    void createInfoPanel();
    void createStepper(uint8_t row, int y, const char* title, const char* hint);
    void createHourStepper(uint8_t row, int centerX, int y);
    void createApplyButton(int y);

    // Lee el estado (con el mutex de datos) y repinta. Solo se llama
    // desde callbacks de LVGL, nunca con el mutex ya tomado.
    void refresh();

    // -/+ : solo cambian el borrador en pantalla (no envian nada)
    void step(uint8_t row, int dir);

    // Boton "Actualizar intervalos": aplica los borradores modificados
    void applyDrafts();

    static void onStepClicked(lv_event_t* e);
    static void onApplyClicked(lv_event_t* e);
    static void onRefreshTimer(lv_timer_t* timer);
    static void setText(lv_obj_t* label, const char* text);

    lv_obj_t* screen = nullptr;

    lv_obj_t* infoValue[INFO_COUNT] = { nullptr };
    lv_obj_t* valueLabel[ROW_COUNT] = { nullptr };
    lv_obj_t* statusLabel = nullptr;
    lv_obj_t* applyBtn = nullptr;

    // Borrador de cada intervalo (segundos). Mientras la fila no se toca
    // sigue al valor aplicado; al tocarla (touched) se queda fijo hasta
    // pulsar "Actualizar" o salir de la pantalla.
    uint32_t draft[ROW_COUNT] = {};
    bool touched[ROW_COUNT] = {};
    bool shownTouched[ROW_COUNT] = {};
    int8_t applyEnabled = -1;

    ButtonCtx buttonCtx[ROW_COUNT * 2];

    lv_timer_t* refreshTimer = nullptr;

    // Para no repintar etiquetas si nada cambio
    uint32_t shownValue[ROW_COUNT] = {};
    uint8_t  lastState = 255;
    bool showResult = false;     // mostrar el resultado de un guardado hecho en ESTA visita
    uint32_t stateSince = 0;
};
