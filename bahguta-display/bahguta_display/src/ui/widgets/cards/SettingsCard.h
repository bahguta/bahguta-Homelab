#pragma once

#include "Card.h"

class SettingsCard : public Card
{
public:

    SettingsCard(lv_obj_t* parent);

    void layout() override;

    void setStatus(const char* text);

private:

    static constexpr int RADIUS = 20;
    static constexpr int PADDING = 20;

    lv_obj_t* titleLabel = nullptr;
    lv_obj_t* statusLabel = nullptr;
};