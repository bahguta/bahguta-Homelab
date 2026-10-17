#pragma once

#include "../../models/DockerInfo.h"

class DockerService
{
    public:
        void init();
        void update();

        DockerInfo& docker();

    private:

        DockerInfo dockerInfo;
    uint32_t seenVersion = 0xFFFFFFFFu;

};