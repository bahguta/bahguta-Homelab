#ifndef BAHGUTA_WIDGET_H
#define BAHGUTA_WIDGET_H

#include <lvgl.h>

class Widget
{
public:

    virtual ~Widget() = default;

    virtual void layout() = 0;

    lv_obj_t *object()
    {
        return root;
    }

protected:

    lv_obj_t *root = nullptr;
};

#endif