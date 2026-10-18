package com.bahguta.bahguta_api.dashboard.dto;

import java.util.List;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@Getter
@Setter
@Builder
@NoArgsConstructor
@AllArgsConstructor
public class DashboardDto {

    private int totalDevices;

    private int onlineDevices;

    private int offlineDevices;
    
    private int totalSensors;

    private int onlineSensors;

    private int offlineSensors;

    private List<DashboardSensorDto> sensors;

}
