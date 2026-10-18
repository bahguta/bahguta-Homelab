#include "StringUtils.h"

#include <string.h>

void StringUtils::copy(char* dst,
                       const char* src,
                       uint16_t size)
{
    strncpy(dst, src, size);

    dst[size - 1] = '\0';
}

bool StringUtils::equals(const char* a,
                         const char* b)
{
    return strcmp(a, b) == 0;
}