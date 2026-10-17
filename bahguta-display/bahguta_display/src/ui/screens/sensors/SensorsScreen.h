#pragma once

#include "../Screen.h"
#include <lvgl.h>

class SensorsScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void show() override;
    void hide() override;
    void destroy() override;

private:

    struct SensorCardUi
    {
        lv_obj_t* name = nullptr;
        lv_obj_t* enabled = nullptr;
        lv_obj_t* id = nullptr;
        lv_obj_t* deviceId = nullptr;
        lv_obj_t* type = nullptr;
        lv_obj_t* interval = nullptr;
    };

    void createContent();
    void update();

    SensorCardUi createCard(
        lv_obj_t* parent,
        int x,
        int y,
        int width,
        int height
    );

    lv_obj_t* addLine(lv_obj_t* parent, lv_color_t color, const lv_font_t* font);

    lv_obj_t* screen;

    static constexpr int CARD_COUNT = 5;
    SensorCardUi cards[CARD_COUNT];
};