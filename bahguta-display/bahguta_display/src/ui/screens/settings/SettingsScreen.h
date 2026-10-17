#pragma once

#include "../Screen.h"
#include <lvgl.h>

class SettingsScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void show() override;
    void hide() override;
    void destroy() override;

    // Solo las filas de red (SSID, senal, IP). Se llama con el mutex de datos
    // TOMADO. No toca los campos editables (si no, se borraria lo que se esta
    // escribiendo).
    void refreshLive();

private:

    void createContent();
    void update();

    lv_obj_t* createRow(lv_obj_t* parent, const char* label);
    lv_obj_t* createInputRow(lv_obj_t* parent, const char* label, bool password);

    lv_obj_t* screen;

    // Estado de la red (solo lectura, se actualiza solo)
    lv_obj_t* ssidValue   = nullptr;
    lv_obj_t* signalValue = nullptr;
    lv_obj_t* ipValue     = nullptr;

    // Editables (WiFi)
    lv_obj_t* wifiSsidInput     = nullptr;
    lv_obj_t* wifiPasswordInput = nullptr;

    lv_obj_t* keyboard = nullptr;
};