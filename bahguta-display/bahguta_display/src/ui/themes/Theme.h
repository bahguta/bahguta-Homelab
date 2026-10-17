#pragma once

#include <lvgl.h>

class Theme
{
public:

    static lv_color_t Background();
    static lv_color_t BottomMenu();
    static lv_color_t Header();
    static lv_color_t Panel();
    static lv_color_t Weather();
    static lv_color_t Border();

    static lv_color_t Primary();

    static lv_color_t Text();
    static lv_color_t TextSecondary();
    
    static lv_color_t Success();
    static lv_color_t Warning();
    static lv_color_t Error();
};