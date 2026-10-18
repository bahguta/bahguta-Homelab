#pragma once

#include "Card.h"

class ConfigCard : public Card
{
public:

    ConfigCard(lv_obj_t* parent);

    void layout() override;

private:

    static constexpr int PADDING = 20;

    lv_obj_t* iconLabel = nullptr;
    lv_obj_t* titleLabel = nullptr;
    lv_obj_t* statusLabel = nullptr;
};