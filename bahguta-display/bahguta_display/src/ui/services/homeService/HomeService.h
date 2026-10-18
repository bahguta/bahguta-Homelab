#pragma once

#include "../../models/HomeInfo.h"

class HomeService
{
public:

    void init();

    void update();

    const HomeInfo& home() const;

private:

    HomeInfo current;
    uint32_t seenVersion = 0xFFFFFFFFu;
};