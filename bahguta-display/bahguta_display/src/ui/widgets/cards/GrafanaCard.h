#pragma once

#include "Card.h"

// Caja de Grafana en HomeScreen: resumen corto (disco, temperatura).
// Al pulsarla abre GrafanaScreen con todas las metricas.
class GrafanaCard : public Card
{
public:

    GrafanaCard(lv_obj_t* parent);

    void layout() override;

    // diskPercent y tempC: -1 = no disponible
    void setSummary(double diskPercent, double tempC);

private:

    static constexpr int PADDING = 20;

    lv_obj_t* iconLabel = nullptr;
    lv_obj_t* titleLabel = nullptr;
    lv_obj_t* diskLabel = nullptr;
    lv_obj_t* tempLabel = nullptr;
};
