#pragma once

#include "Card.h"

class SystemCard : public Card
{
public:

    SystemCard(lv_obj_t* parent);

    void layout() override;

    // Ancho real de la card en px. Hay que llamarlo ANTES de layout():
    // en ese momento LVGL aun puede devolver 0 como ancho y las dos
    // columnas quedaban apinadas a la izquierda.
    void setCardWidth(int width) { cardWidth = width; }

    //void setTabletCpu(double cpu);
    void setTabletUptime(uint32_t uptime);
    void setTabletRam(double ram);

    void setServerRam(double ram);
    void setServerCpu(double cpu);
    void setServerUptime(uint32_t uptime);


private:

    void layoutCompact();

    int cardWidth = 0;
    bool compact = false;

    static constexpr int RADIUS = 20;
    static constexpr int PADDING = 20;


    lv_obj_t* title = nullptr;


    // ========================================================
    // SERVER
    // ========================================================

    lv_obj_t* serverTitle = nullptr;
    lv_obj_t* serverCpuLabel = nullptr;
    lv_obj_t* serverRamLabel = nullptr;
    lv_obj_t* serverUptimeLabel = nullptr;


    // ========================================================
    // TABLET
    // ========================================================

    lv_obj_t* tabletTitle = nullptr;
    lv_obj_t* tabletCpuLabel = nullptr;
    lv_obj_t* tabletRamLabel = nullptr;
    lv_obj_t* tabletUptimeLabel = nullptr;
};