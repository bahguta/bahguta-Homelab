#include "HttpService.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

void HttpService::init()
{
}

HttpResponse HttpService::get(const char* url)
{
    HttpResponse response;

    Serial.println();
    Serial.println("========== HTTP GET ==========");
    Serial.printf("URL: %s\n", url);

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("HTTP GET: WIFI NOT CONNECTED");
        return response;
    }

    HTTPClient http;

    http.setTimeout(10000);

    if (!http.begin(url))
    {
        Serial.println("HTTP GET: BEGIN FAILED");
        return response;
    }

    int code = http.GET();

    response.code = code;

    if (code <= 0)
    {
        Serial.printf(
            "HTTP GET: ERROR = %s\n",
            http.errorToString(code).c_str()
        );

        http.end();
        return response;
    }

    response.body = http.getString();

    response.success =
        code >= 200 &&
        code < 300;

    Serial.printf(
        "HTTP GET: CODE = %d\n",
        response.code
    );

    Serial.printf(
        "HTTP GET: BODY LENGTH = %d\n",
        response.body.length()
    );

    http.end();

    return response;
}

HttpResponse HttpService::post(
    const char* url,
    const char* json)
{
    HttpResponse response;

    if (WiFi.status() != WL_CONNECTED)
        return response;

    HTTPClient http;

    http.setTimeout(10000);

    if (!http.begin(url))
        return response;

    http.addHeader(
        "Content-Type",
        "application/json"
    );

    int code = http.POST(json);

    response.code = code;

    if (code > 0)
    {
        response.body = http.getString();

        response.success =
            code >= 200 &&
            code < 300;
    }

    http.end();

    return response;
}