#pragma once

#include <Arduino.h>

struct DockerContainerInfo
{
    String id;
    String name;
    String image;
    String status;
    String state;
    String ports;
};