#pragma once

#include "../storage/Storage.h"
#include "../model/Measurement.h"
#include "../sensors/TemperatureSensor.h"

class ApiClient
{
public:

    void registerDevice();
    void registerMeasurement(const Measurement& measurement);
    void setTemperatureSensor(TemperatureSensor* sensor);
    void heartbeat();

private:

    
    void processConfigurationResponse(const String& response);

    Storage storage;
    TemperatureSensor* temperatureSensor = nullptr;

    
};

inline void ApiClient::setTemperatureSensor(TemperatureSensor* sensor)
{
    temperatureSensor = sensor;
}