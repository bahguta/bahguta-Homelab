#pragma once

#include <Arduino.h>
#include "DockerContainerInfo.h"

struct DockerInfo
{
    int online = 0;
    int total = 0;
    bool connected = false;

    DockerContainerInfo containers[32];
    int containerCount = 0;
};