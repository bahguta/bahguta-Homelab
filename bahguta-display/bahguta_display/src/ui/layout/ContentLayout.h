#pragma once
#include <lvgl.h>

class ContentLayout
{
public:

    static constexpr int LEFT = 20;
    static constexpr int RIGHT = 20;
    static constexpr int TOP = 15;
    static constexpr int GAP = 15;

    static constexpr int WEATHER_HEIGHT = 420;
    static constexpr int CARD_HEIGHT = 150;

    // Titulo comun de pantalla (alineado a la izquierda, mismo margen que Home)
    static constexpr int TITLE_TOP = 15;
    static constexpr int TITLE_HEIGHT = 40;
    static constexpr int TITLE_GAP = 12;
    // Y donde empieza el contenido de una pantalla con titulo
    static constexpr int CONTENT_TOP = TITLE_TOP + TITLE_HEIGHT + TITLE_GAP;

    // Ancho real del area de contenido de una pantalla (igual que Home)
    static int screenContentWidth(lv_obj_t* screen);

    // Crea el titulo estandar de pantalla; devuelve el label
    static lv_obj_t* createTitle(lv_obj_t* screen, const char* text);

    static int contentWidth(int bodyWidth);

    static int cardWidth(int bodyWidth);

    static int cardWidth(int bodyWidth, int columns);
};