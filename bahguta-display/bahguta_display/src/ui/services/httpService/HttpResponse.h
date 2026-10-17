#pragma once

#include <Arduino.h>

struct HttpResponse
{
    int code = -1;
    bool success = false;

    String body;
};
