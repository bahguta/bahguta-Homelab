#include "CasaChart.h"

#include "../../../fonts/Fonts.h"
#include "../../themes/Theme.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


// Margenes interiores de la zona de dibujo (px)
static constexpr int PAD_LEFT   = 56;   // etiquetas del eje Y
static constexpr int PAD_RIGHT  = 22;
static constexpr int PAD_TOP    = 40;   // titulo
static constexpr int PAD_BOTTOM = 32;   // etiquetas del eje X

// La ventana sigue siendo de 24 h pero se desplaza hacia el futuro: se quitan estas
// horas por detras y se dejan por delante (vacias) para ver mejor las horas proximas.
static constexpr int AHEAD_HOURS = 3;

static constexpr int LINE_WIDTH = 3;
static constexpr int CURVE_SUBDIVISIONS = 8;

// Si entre dos puntos hay mas de GAP_FACTOR tramos sin datos, la linea se corta
static constexpr float GAP_FACTOR = 2.5f;


void CasaChart::create(lv_obj_t* parent, int width, int height)
{
    root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, width, height);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(root, Theme::Panel(), 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(root, 16, 0);

    for (uint8_t i = 0; i < CasaHistory::MAX_SERIES; i++)
        colors[i] = Theme::TextSecondary();

    lv_obj_add_event_cb(root, onDraw, LV_EVENT_DRAW_MAIN, this);
}


void CasaChart::setData(const CasaHistory& history)
{
    data = history;

    if (root)
        lv_obj_invalidate(root);
}


void CasaChart::setSeriesColor(uint8_t index, lv_color_t color)
{
    if (index >= CasaHistory::MAX_SERIES)
        return;

    colors[index] = color;

    if (root)
        lv_obj_invalidate(root);
}


void CasaChart::onDraw(lv_event_t* e)
{
    CasaChart* self = static_cast<CasaChart*>(lv_event_get_user_data(e));
    lv_layer_t* layer = lv_event_get_layer(e);

    if (self && layer)
        self->draw(layer);
}


void CasaChart::drawText(
    lv_layer_t* layer,
    const char* text,
    int x, int y, int w,
    const lv_font_t* font,
    lv_color_t color,
    lv_text_align_t align)
{
    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);

    dsc.color = color;
    dsc.font = font;
    dsc.text = text;
    dsc.text_local = 1;          // el dibujo es diferido: LVGL copia el texto
    dsc.align = align;

    lv_area_t area;
    area.x1 = x;
    area.y1 = y;
    area.x2 = x + w - 1;
    area.y2 = y + font->line_height;

    lv_draw_label(layer, &dsc, &area);
}


// Curva suave (Catmull-Rom) por los puntos de un tramo continuo + un punto en cada medicion
void CasaChart::drawRun(
    lv_layer_t* layer,
    const int* xs, const int* ys, int count,
    lv_color_t color,
    int minY, int maxY)
{
    lv_draw_line_dsc_t line;
    lv_draw_line_dsc_init(&line);
    line.color = color;
    line.width = LINE_WIDTH;
    line.opa = LV_OPA_COVER;
    line.round_start = 1;
    line.round_end = 1;

    for (int i = 0; i + 1 < count; i++)
    {
        const float p0x = (float)xs[i > 0 ? i - 1 : i];
        const float p0y = (float)ys[i > 0 ? i - 1 : i];
        const float p1x = (float)xs[i];
        const float p1y = (float)ys[i];
        const float p2x = (float)xs[i + 1];
        const float p2y = (float)ys[i + 1];
        const float p3x = (float)xs[i + 2 < count ? i + 2 : i + 1];
        const float p3y = (float)ys[i + 2 < count ? i + 2 : i + 1];

        float prevX = p1x;
        float prevY = p1y;

        for (int s = 1; s <= CURVE_SUBDIVISIONS; s++)
        {
            const float t = (float)s / CURVE_SUBDIVISIONS;
            const float t2 = t * t;
            const float t3 = t2 * t;

            float x = 0.5f * ((2.0f * p1x) + (-p0x + p2x) * t
                    + (2.0f * p0x - 5.0f * p1x + 4.0f * p2x - p3x) * t2
                    + (-p0x + 3.0f * p1x - 3.0f * p2x + p3x) * t3);

            float y = 0.5f * ((2.0f * p1y) + (-p0y + p2y) * t
                    + (2.0f * p0y - 5.0f * p1y + 4.0f * p2y - p3y) * t2
                    + (-p0y + 3.0f * p1y - 3.0f * p2y + p3y) * t3);

            // La curva no se sale de la zona de dibujo
            if (y < (float)minY) y = (float)minY;
            if (y > (float)maxY) y = (float)maxY;

            line.p1.x = (int32_t)prevX;
            line.p1.y = (int32_t)prevY;
            line.p2.x = (int32_t)x;
            line.p2.y = (int32_t)y;
            lv_draw_line(layer, &line);

            prevX = x;
            prevY = y;
        }
    }
}


void CasaChart::draw(lv_layer_t* layer)
{
    lv_area_t box;
    lv_obj_get_coords(root, &box);

    // Titulo
    drawText(layer, "HISTÓRICO 24 h   ·   TEMPERATURA (°C)",
             box.x1 + 20, box.y1 + 12, 420,
             &montserrat_28_es_14, Theme::TextSecondary(), LV_TEXT_ALIGN_LEFT);

    const int px0 = box.x1 + PAD_LEFT;
    const int px1 = box.x2 - PAD_RIGHT;
    const int py0 = box.y1 + PAD_TOP;
    const int py1 = box.y2 - PAD_BOTTOM;
    const int plotW = px1 - px0;
    const int plotH = py1 - py0;

    // Ventana visible: la que manda el backend desplazada AHEAD_HOURS hacia el futuro
    const time_t viewFrom = (time_t)data.from + (time_t)AHEAD_HOURS * 3600;
    const time_t viewTo   = (time_t)data.to   + (time_t)AHEAD_HOURS * 3600;
    const float halfStep  = (float)data.stepSeconds / 2.0f;

    // ---- rango de temperatura de las series (solo puntos visibles) ----
    float tMin = 1000.0f;
    float tMax = -1000.0f;
    int totalPoints = 0;

    for (uint8_t s = 0; s < data.seriesCount; s++)
    {
        const HistorySeries& series = data.series[s];

        for (uint8_t i = 0; i < series.count; i++)
        {
            const float t = series.points[i].t;

            if (isnan(t))
                continue;

            if ((float)series.points[i].ts + halfStep < (float)viewFrom)
                continue;   // ya fuera de la ventana por la izquierda

            if (t < tMin) tMin = t;
            if (t > tMax) tMax = t;
            totalPoints++;
        }
    }

    if (!data.available || totalPoints == 0 || data.to <= data.from)
    {
        drawText(layer,
                 data.available ? "Sin mediciones en las últimas 24 h" : "Cargando histórico...",
                 box.x1, box.y1 + (box.y2 - box.y1) / 2 - 10, box.x2 - box.x1,
                 &montserrat_28_es_16, Theme::TextSecondary(), LV_TEXT_ALIGN_CENTER);
        return;
    }

    // Margen minimo de 2 C para que una linea casi plana no se estire
    if (tMax - tMin < 2.0f)
    {
        const float mid = (tMax + tMin) / 2.0f;
        tMin = mid - 1.0f;
        tMax = mid + 1.0f;
    }

    // Paso "bonito" de la rejilla
    const float rawStep = (tMax - tMin) / 4.0f;
    float step = 5.0f;
    if (rawStep <= 0.5f) step = 0.5f;
    else if (rawStep <= 1.0f) step = 1.0f;
    else if (rawStep <= 2.0f) step = 2.0f;

    const float yMin = floorf(tMin / step) * step;
    float yMax = ceilf(tMax / step) * step;
    if (yMax <= yMin) yMax = yMin + step;

    auto yOf = [&](float t) -> int
    {
        return py1 - (int)((t - yMin) / (yMax - yMin) * (float)plotH);
    };

    // ---- rejilla horizontal + etiquetas Y ----
    lv_draw_line_dsc_t grid;
    lv_draw_line_dsc_init(&grid);
    grid.color = Theme::Border();
    grid.width = 1;
    grid.opa = LV_OPA_COVER;

    for (float v = yMin; v <= yMax + 0.001f; v += step)
    {
        const int y = yOf(v);

        grid.p1.x = px0;
        grid.p1.y = y;
        grid.p2.x = px1;
        grid.p2.y = y;
        lv_draw_line(layer, &grid);

        char label[12];
        if (step >= 1.0f)
            snprintf(label, sizeof(label), "%.0f", v);
        else
            snprintf(label, sizeof(label), "%.1f", v);

        drawText(layer, label, box.x1 + 6, y - 8, PAD_LEFT - 14,
                 &montserrat_28_es_14, Theme::TextSecondary(), LV_TEXT_ALIGN_RIGHT);
    }

    // ---- eje X: una marca y una etiqueta (solo la hora) por cada hora, hora local ----
    const float span = (float)(viewTo - viewFrom);

    lv_draw_line_dsc_t tick;
    lv_draw_line_dsc_init(&tick);
    tick.color = Theme::Border();
    tick.width = 1;
    tick.opa = LV_OPA_COVER;

    time_t t = viewFrom + (3600 - (viewFrom % 3600)) % 3600;

    for (; t <= viewTo; t += 3600)
    {
        struct tm lt;
        localtime_r(&t, &lt);

        const int x = px0 + (int)((float)(t - viewFrom) / span * (float)plotW);

        tick.p1.x = x;
        tick.p1.y = py1;
        tick.p2.x = x;
        tick.p2.y = py1 + 5;
        lv_draw_line(layer, &tick);

        char label[8];
        snprintf(label, sizeof(label), "%02d", lt.tm_hour);

        // Solo la hora (08, 09, 10...): una etiqueta por hora en una sola fila
        drawText(layer, label, x - 14, py1 + 8, 28,
                 &montserrat_28_es_14, Theme::TextSecondary(), LV_TEXT_ALIGN_CENTER);
    }

    // ---- una linea por habitacion ----
    for (uint8_t s = 0; s < data.seriesCount; s++)
    {
        const HistorySeries& series = data.series[s];

        int xs[HistorySeries::MAX_POINTS];
        int ys[HistorySeries::MAX_POINTS];
        int n = 0;
        uint32_t lastTs = 0;

        for (uint8_t i = 0; i <= series.count; i++)
        {
            const bool valid = (i < series.count) && !isnan(series.points[i].t);

            // Hueco largo (sensor sin reportar): se cierra el tramo actual
            const bool gap = valid && n > 0 &&
                (float)(series.points[i].ts - lastTs) > GAP_FACTOR * (float)data.stepSeconds;

            if ((!valid || gap) && n > 0)
            {
                drawRun(layer, xs, ys, n, colors[s], py0, py1);
                n = 0;
            }

            if (!valid)
                continue;

            // El punto se dibuja en el centro de su tramo
            float pos = ((float)series.points[i].ts + halfStep - (float)viewFrom) / span;

            // Fuera de la ventana por la izquierda: se descarta (cortaria la linea)
            if (pos < 0.0f)
            {
                if (n > 0)
                {
                    drawRun(layer, xs, ys, n, colors[s], py0, py1);
                    n = 0;
                }
                continue;
            }
            if (pos > 1.0f) pos = 1.0f;

            xs[n] = px0 + (int)(pos * (float)plotW);
            ys[n] = yOf(series.points[i].t);
            n++;
            lastTs = series.points[i].ts;
        }
    }
}
