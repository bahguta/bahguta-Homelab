package com.bahguta.bahguta_api.dashboard.dto;

import java.time.LocalDateTime;
import java.util.UUID;

import com.bahguta.bahguta_api.sensor.entity.SensorType;

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
public class DashboardSensorDto {

    private UUID deviceId;

    private String deviceName;

    private UUID sensorId;

    private String sensorName;

    private SensorType sensorType;

    private boolean enabled;

    private Double temperature;

    private Double humidity;

    private Double pressure;

    private Double value;

    private LocalDateTime lastMeasurement;

}