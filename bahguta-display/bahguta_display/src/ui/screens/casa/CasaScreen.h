#pragma once

#include "../Screen.h"
#include "../../models/CasaInfo.h"
#include "../../widgets/charts/CasaChart.h"
#include <lvgl.h>

class CasaScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void show() override;
    void hide() override;
    void destroy() override;

    void update();

    static constexpr int ROOM_COUNT = 9;

private:

    struct RoomBox
    {
        const char* matchInclude;
        const char* matchExclude;
        int x, y, w, h;
        bool hasSensor;

        lv_obj_t* box = nullptr;
        lv_obj_t* tempLabel = nullptr;
        lv_obj_t* humLabel = nullptr;
    };

    // Plano segun el dibujo de la casa (720 x 300 px); x, y, ancho, alto
    RoomBox rooms[ROOM_COUNT] = {
        { "despacho",  nullptr,       0,   0, 130, 208, true  },
        { "infantil",  "bano",      130,   0, 131, 248, true  },
        { "dormitorio","infantil",  261,   0, 122, 245, true  },
        { "bano",      "infantil",  383,   0,  71, 164, false },
        { "salon",     nullptr,     454,   0, 266, 173, true  },
        { "recibidor", nullptr,     383, 166,  71,  75, false },
        { "infantil",  "dormitorio",  0, 208,  88,  90, false }, // baño infantil
        { "pasillo",   nullptr,      88, 248, 421,  52, false },
        { "cocina",    nullptr,     509, 173, 211, 125, true  },
    };

    // Todos los bloques miden lo mismo de ancho y llevan el mismo margen (GAP) en vertical
    static constexpr int CONTENT_W = 760;      // = ancho de contenido de Home
    static constexpr int PLAN_REF_W = 720;     // ancho para el que estan definidas las coordenadas de ROOMS
    static constexpr int PLAN_H = 300;
    int contentW = CONTENT_W;                  // ancho real de contenido (se mide en createContent)
    static constexpr int GAP = 15;             // = ContentLayout::GAP

    static const char* roomLabel(int index);

    void createContent();
    void createFloorPlan(lv_obj_t* parent);
    void createInfoPanel(lv_obj_t* parent);
    void createChart(lv_obj_t* parent);
    void updateChart();

    // Color de la linea (y del titulo) de la n-esima habitacion con sensor
    static lv_color_t sensorColor(int ordinal);
    int sensorOrdinalFor(const char* roomName) const;

    lv_obj_t* screen;
    lv_obj_t* avgTempValue = nullptr;
    lv_obj_t* avgHumidityValue = nullptr;

    CasaChart chart;
    uint32_t chartVersion = 0xFFFFFFFFu;
    uint32_t lastHistoryRequest = 0;
    static constexpr uint32_t HISTORY_REFRESH_MS = 5UL * 60UL * 1000UL;
    static constexpr int CHART_HEIGHT = 300;
};
