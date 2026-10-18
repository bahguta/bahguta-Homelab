#pragma once

#include <lvgl.h>
#include "../../models/CasaHistory.h"

/*
 * Grafica de historico de temperatura: una linea suave con puntos por habitacion.
 *   eje X: horas (ventana del historico)   eje Y: temperatura (C)
 * Se dibuja a mano sobre un lv_obj (sin lv_chart, que no hace curvas suaves).
 * Si hay un hueco largo sin datos (sensor sin reportar) la linea se corta.
 */
class CasaChart
{
public:

    void create(lv_obj_t* parent, int width, int height);

    lv_obj_t* object() const { return root; }

    // Copia los datos y repinta
    void setData(const CasaHistory& history);

    // Color de la linea de la serie i (mismo orden que history.series)
    void setSeriesColor(uint8_t index, lv_color_t color);

private:

    static void onDraw(lv_event_t* e);
    void draw(lv_layer_t* layer);

    void drawText(
        lv_layer_t* layer,
        const char* text,
        int x, int y, int w,
        const lv_font_t* font,
        lv_color_t color,
        lv_text_align_t align);

    void drawRun(
        lv_layer_t* layer,
        const int* xs, const int* ys, int count,
        lv_color_t color,
        int minY, int maxY);

    lv_obj_t* root = nullptr;

    CasaHistory data;
    lv_color_t colors[CasaHistory::MAX_SERIES];
};
