#pragma once

#include <lvgl.h>

class Footer
{
public:

    Footer(lv_obj_t *parent);

    lv_obj_t *object();

    void setDate();

    void setVersion(const char *version);

    void layout();

    void update();

    static constexpr int HEIGHT = 35;

private:

    
    lv_obj_t *developer;
    lv_obj_t *root;
    lv_obj_t *date;
    lv_obj_t *version;

    char shownVersion[16] = "";
};