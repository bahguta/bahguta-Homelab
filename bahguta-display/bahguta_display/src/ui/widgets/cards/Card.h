#pragma once

#include "../Widget.h"
#include "../../screens/ScreenType.h"

class Card : public Widget
{
public:

    Card(lv_obj_t *parent);
    //void setScreen(ScreenType screen);
    void setClickScreen(ScreenType screen);

protected:

    lv_obj_t* card;

    lv_obj_t* createIconChip(const char* symbol, lv_color_t color);
    lv_obj_t* createProgressBar(lv_obj_t* parent, int width, lv_color_t color);

private:

    struct ClickData
    {
        Card* card;
        ScreenType screen;
        lv_point_t pressPoint;      // donde empezo el toque (para distinguir tap de swipe)
        bool pressed;
    };

    static void clickEvent(lv_event_t* event);
    static void pressEvent(lv_event_t* event);

    void onClick(ScreenType screen);
};