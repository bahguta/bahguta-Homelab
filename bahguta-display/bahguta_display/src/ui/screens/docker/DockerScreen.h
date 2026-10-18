#pragma once

#include "../Screen.h"
#include <lvgl.h>

class DockerScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void show() override;
    void hide() override;
    void destroy() override;
    void update();

private:

    void createContent();

    // Alto de la lista = lo que queda de pantalla (se mide; el body tiene padding)
    int listHeight() const;

    void createContainerList();

    void updateContainerList();

    static void toggleEvent(lv_event_t* e);

    lv_obj_t* screen;

    lv_obj_t* list = nullptr; 
    lv_obj_t* containerRows[32] = {}; 
    lv_obj_t* statusDots[32] = {}; 
    lv_obj_t* nameLabels[32] = {}; 
    lv_obj_t* stateLabels[32] = {}; 
    
    lv_obj_t* switches[32] = {};

    char containerIds[32][40] = {};

    int rowCount = 0;

    // Ultimo estado pintado de cada fila (-1 = aun no pintada): evita llamadas
    // a LVGL (y repintados) cuando el dato no ha cambiado
    int8_t shownRunning[32] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
    

};