package com.bahguta.bahguta_api.dashboard.service;

import com.bahguta.bahguta_api.dashboard.dto.DashboardDto;

public interface DashboardService {

    DashboardDto getDashboardLatestMeasurements();
    DashboardDto getDashboardAllMeasurements();

}