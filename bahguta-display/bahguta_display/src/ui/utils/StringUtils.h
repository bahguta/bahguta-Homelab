#pragma once

#include <Arduino.h>

class StringUtils
{
public:

    static void copy(char* dst,
                     const char* src,
                     uint16_t size);

    static bool equals(const char* a,
                       const char* b);
};