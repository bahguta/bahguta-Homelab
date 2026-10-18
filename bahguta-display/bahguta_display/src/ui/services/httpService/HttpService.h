#pragma once

#include "HttpResponse.h"

class HttpService
{
public:

    void init();

    HttpResponse get(const char* url);

    HttpResponse post(
        const char* url,
        const char* json,
        const char* apiKey = nullptr);
};