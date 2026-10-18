#pragma once

#include "../Screen.h"
#include <lvgl.h>

/*
 * Pantalla DISPOSITIVOS (device + sensor unificados).
 *
 * En la tablet un "device" es la placa ESP32-C3 con su sensor DHT22:
 * una sola card por device con los datos de ambos. Caben 5 cards
 * apiladas en el area de contenido (800 x 970).
 */
class DevicesScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void show() override;
    void hide() override;
    void destroy() override;

    // Repinta desde los datos del ApiService. Se llama con el mutex de
    // datos TOMADO (MainLayout::update y MainLayout::setBody lo hacen).
    void update();

private:

    static constexpr int CARD_COUNT = 5;
    static constexpr int LINES = 6;

    struct DeviceCardUi
    {
        lv_obj_t* box = nullptr;
        lv_obj_t* name = nullptr;
        lv_obj_t* status = nullptr;
        lv_obj_t* left[LINES]  = { nullptr };
        lv_obj_t* right[LINES] = { nullptr };
    };

    void createContent();

    DeviceCardUi createCard(
        lv_obj_t* parent,
        int x,
        int y,
        int width,
        int height
    );

    lv_obj_t* addLine(
        lv_obj_t* parent,
        int x,
        int y,
        int width,
        lv_color_t color
    );

    // Solo toca LVGL si el texto cambio (evita redibujar sin necesidad)
    static void setText(lv_obj_t* label, const char* text);

    lv_obj_t* screen = nullptr;
    lv_obj_t* emptyLabel = nullptr;

    DeviceCardUi cards[CARD_COUNT];
};
