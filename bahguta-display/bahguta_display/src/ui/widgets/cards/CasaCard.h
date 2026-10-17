#pragma once

#include "Card.h"

class CasaCard : public Card
{
public:

    CasaCard(lv_obj_t* parent);

    void layout() override;

    void setTemperature(double temperature);
    // available = false -> "--" (ninguna habitacion con dato)
    void setTemperature(double temperature, bool available);

    // Temperatura y humedad medias en la misma linea ("22.2 °C   64 %"); sin dato -> "--"
    void setClimate(double temperature, bool temperatureAvailable,
                    double humidity, bool humidityAvailable);
    void setRooms(int rooms);

private:

    static constexpr int RADIUS = 20;
    static constexpr int PADDING = 20;

    lv_obj_t* titleLabel = nullptr;
    lv_obj_t* temperatureLabel = nullptr;
    lv_obj_t* roomsLabel = nullptr;
};