#pragma once

#include "ClockInfo.h"
#include "WeatherInfo.h"
#include "NetworkInfo.h"
#include "SystemInfo.h"
#include "DeviceInfo.h"
#include "SensorInfo.h"
#include "DockerInfo.h"
#include"BackendServerInfo.h"
#include "RedInfo.h"
#include "GrafanaInfo.h"

struct HomeInfo
{
    ClockInfo clock;

    WeatherInfo weather;

    NetworkInfo network;

    SystemInfo system;

    DeviceInfo devices;

    SensorInfo sensors;

    DockerInfo docker;

    BackendServerInfo server;

    RedInfo red;

    GrafanaInfo grafana;

    bool success = false;
};