#pragma once

#include "../Screen.h"
#include <Arduino.h>
#include <lvgl.h>
#include <stdint.h>
#include <cstdint>

class SplashScreen : public Screen
{
public:

    void create(lv_obj_t* root) override;
    void show() override;
    void hide() override;
    void destroy() override;
    void setStatus(const char* text);
    void setProgress(int value);
    void play();
    void nextStep();
    void refreshVersion();
    bool finished() const;

    // Fade-out del contenido y transicion suave hacia 'next'.
    // El splash se borra solo al terminar la transicion.
    void exit(lv_obj_t* next);

private:

    static void animationTimer(lv_timer_t *timer);

    // Callbacks de animacion
    static void opaExec(void* var, int32_t v);
    static void transYExec(void* var, int32_t v);
    static void breathExec(void* var, int32_t v);
    static void progressExec(void* var, int32_t v);
    static void exitExec(void* var, int32_t v);
    static void exitReady(lv_anim_t* a);
    static void houseExec(void* var, int32_t v);

    void createHouseIcon();

    static void enter(lv_obj_t* o, uint32_t delay, int32_t dy);
    void setContentOpa(lv_opa_t opa);

    lv_obj_t* screen   = nullptr;
    lv_obj_t* logo     = nullptr;
    lv_obj_t* version  = nullptr;
    lv_obj_t* status   = nullptr;
    lv_obj_t* progress = nullptr;
    lv_obj_t* percent  = nullptr;
    lv_obj_t* logoDeveloper = nullptr;
    lv_obj_t* footerBar = nullptr;

    // Icono casa + wifi (dibujado con lv_line / lv_arc)
    lv_obj_t* houseBox  = nullptr;
    lv_obj_t* houseLine = nullptr;
    lv_obj_t* wifiDot   = nullptr;
    lv_obj_t* wifiArc[3] = { nullptr, nullptr, nullptr };

    lv_point_precise_t housePts[8];
    int32_t            houseSegLen[5];
    int32_t            houseTotal = 0;

    lv_obj_t* nextScreen = nullptr;

    uint8_t step    = 0;
    int32_t shown   = 0;
    bool    started = false;
    bool    exiting = false;
};
