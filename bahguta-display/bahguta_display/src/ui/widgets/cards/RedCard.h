#pragma once

#include "Card.h"

// Caja de Red en HomeScreen: poca informacion (cuantos equipos hay conectados).
// Al pulsarla abre RedScreen con la lista completa.
class RedCard : public Card
{
public:

    RedCard(lv_obj_t* parent);

    void layout() override;

    // total = equipos conectados; available = false si el backend aun no envia datos
    void setClients(int total, bool available);

private:

    static constexpr int PADDING = 20;

    lv_obj_t* iconLabel = nullptr;
    lv_obj_t* titleLabel = nullptr;
    lv_obj_t* countLabel = nullptr;
    lv_obj_t* subLabel = nullptr;
};
