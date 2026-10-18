#pragma once

#include "../Screen.h"
#include <lvgl.h>
#include "../../models/RedInfo.h"

/*
 * RED: lista de equipos conectados al router (wifi, cable...).
 * Los datos vienen del backend dentro del dashboard (HomeInfo.red).
 */
class RedScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void show() override;
    void hide() override;
    void destroy() override;

    // Se llama con el mutex de datos TOMADO (MainLayout lo hace)
    void update();

private:

    void createContent();
    void rebuildList(const RedInfo& red);

    lv_obj_t* screen = nullptr;
    lv_obj_t* countLabel = nullptr;
    lv_obj_t* list = nullptr;

    uint32_t lastSignature = 0xFFFFFFFFu;

    static constexpr int MAX_ROWS = 40;
};
