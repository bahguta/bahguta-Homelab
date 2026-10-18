#pragma once

#include "../Screen.h"
#include <lvgl.h>

/*
 * GRAFANA: metricas del servidor (discos, memoria, CPU, temperatura...).
 * Los datos vienen del backend dentro del dashboard (HomeInfo.grafana).
 */
class GrafanaScreen : public Screen
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

    lv_obj_t* createRow(lv_obj_t* parent, const char* label);
    void createSection(lv_obj_t* parent, const char* text);

    // Una particion como barra tipo Windows: titulo + total, barra y leyenda
    struct DiskView
    {
        lv_obj_t* bar = nullptr;
        lv_obj_t* total = nullptr;
        lv_obj_t* used = nullptr;
        lv_obj_t* free = nullptr;
    };

    DiskView createDisk(lv_obj_t* parent, const char* title);
    void updateDisk(DiskView& disk, double totalGb, double usedGb, double freeGb);

    static void setText(lv_obj_t* label, const char* text);

    lv_obj_t* screen = nullptr;

    DiskView rootDisk;
    DiskView storageDisk;

    lv_obj_t* cpuValue = nullptr;
    lv_obj_t* ramValue = nullptr;
    lv_obj_t* tempValue = nullptr;
};
