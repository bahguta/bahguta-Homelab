#pragma once

#include "Card.h"

class SensorCard : public Card
{
public:

    SensorCard(lv_obj_t* parent);

    void layout() override;

    void setOnline(int online, int total);

private:

    static constexpr int PADDING = 16;

    lv_obj_t* iconChip = nullptr;
    lv_obj_t* title = nullptr;
    lv_obj_t* value = nullptr;
    lv_obj_t* caption = nullptr;
    lv_obj_t* barBg = nullptr;
    lv_obj_t* barFill = nullptr;
};
