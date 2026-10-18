#pragma once

#include "../Widget.h"
#include "../../screens/ScreenType.h"

class BottomMenu : public Widget
{
public:

    BottomMenu(lv_obj_t* parent);

    void layout() override;
    void select(ScreenType screen);
    void update();

    static constexpr int HEIGHT = 130;

private:

    struct ItemData
    {
        BottomMenu* menu;
        ScreenType screen;
    };

    lv_obj_t* root;
    lv_obj_t* container;

    lv_obj_t* home;
    lv_obj_t* weather;
    lv_obj_t* docker;
    lv_obj_t* devices;
    lv_obj_t* config;
    lv_obj_t* casa;
    lv_obj_t* system;
    lv_obj_t* settings;
    lv_obj_t* red;
    lv_obj_t* grafana;

    lv_obj_t* createItem(
        const char* icon,
        const char* text,
        ScreenType screen);

    void select(lv_obj_t* item);

    static void clickEvent(lv_event_t* e);

    void onItemPressed(
        lv_obj_t* item,
        ScreenType screen);
};