#pragma once

#include <DHT.h>
#include "../model/Measurement.h"
#include "../storage/Storage.h"

class TemperatureSensor
{
public:

    void setup();

    void loop();

    Measurement getMeasurement();

private:

    DHT dht = DHT(4, DHT22);
    Storage storage;

    float temperature = NAN;
    float humidity = NAN;
    unsigned long lastRead = 0;
};