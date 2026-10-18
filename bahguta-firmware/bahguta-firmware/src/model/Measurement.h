#pragma once

struct Measurement
{
    String sensorId;

    String sensorName;

    float temperature = NAN;

    float humidity = NAN;

    float pressure = NAN;

    float value = NAN;
};