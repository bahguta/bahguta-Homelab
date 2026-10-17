#pragma once

#include "Card.h"

class DockerCard : public Card
{
public:

    DockerCard(lv_obj_t* parent);

    void layout() override;

    void setRunning(int running, int total);

private:

    static constexpr int PADDING = 16;

    lv_obj_t* iconChip = nullptr;
    lv_obj_t* titleLabel = nullptr;
    lv_obj_t* runningLabel = nullptr;
    lv_obj_t* statusLabel = nullptr;
    lv_obj_t* barBg = nullptr;
    lv_obj_t* barFill = nullptr;
};
