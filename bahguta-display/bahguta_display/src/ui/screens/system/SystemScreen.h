#pragma once

#include "../Screen.h"
#include <lvgl.h>

/*
 * SISTEMA: informacion del sistema (solo lectura).
 *   Servidor: uptime, CPU, RAM
 *   Tablet:   uptime, RAM
 *   Conexion: URL del backend, nombre del dispositivo, zona horaria
 *
 * Los ajustes modificables estan en CONFIG (intervalos) y AJUSTES (WiFi,
 * apagado y reinicio).
 */
class SystemScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void show() override;
    void hide() override;
    void destroy() override;

    // Repinta desde los datos actuales. Se llama con el mutex de datos
    // TOMADO (MainLayout::update y MainLayout::setBody lo hacen).
    void update();

private:

    void createContent();

    lv_obj_t* createRow(lv_obj_t* parent, const char* label);
    void createHeader(lv_obj_t* parent, const char* text);

    static void setText(lv_obj_t* label, const char* text);

    lv_obj_t* screen = nullptr;

    lv_obj_t* serverUptimeValue = nullptr;
    lv_obj_t* serverCpuValue = nullptr;
    lv_obj_t* serverRamValue = nullptr;

    lv_obj_t* tabletUptimeValue = nullptr;
    lv_obj_t* tabletRamValue = nullptr;

    lv_obj_t* backendUrlValue = nullptr;
    lv_obj_t* deviceNameValue = nullptr;
    lv_obj_t* timezoneValue = nullptr;
};
