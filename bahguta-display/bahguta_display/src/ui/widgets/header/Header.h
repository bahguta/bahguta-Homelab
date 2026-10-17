#pragma once

#include "../Widget.h"


class Header  : public Widget {
public:

    Header(lv_obj_t *parent);

    void setTime();

    void setWifi(bool connected);

    void setBackend(bool connected);

    void setDocker(bool connected);

    void setBattery(int percent);

    void layout();

    static constexpr int HEIGHT = 70;

    void update();

private:

    

    lv_obj_t *logo;

    lv_obj_t *clock;

    lv_obj_t *wifi;

    lv_obj_t *backend;

    lv_obj_t *docker;

    lv_obj_t *battery;
};