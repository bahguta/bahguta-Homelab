#pragma once
#include <Arduino.h>
#include <lvgl.h>

class Screen
{
public:
    virtual void create(lv_obj_t* rootScreen);
    virtual void destroy() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
};
