package com.bahguta.bahguta_api.measurement.dto;

import java.time.LocalDateTime;
import java.util.UUID;

import com.bahguta.bahguta_api.sensor.dto.SensorDto;

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
public class MeasurementDto {

    private UUID id;

    private SensorDto sensor;

    private Double temperature;

    private Double humidity;

    private Double pressure;

    private Double valor;

    private LocalDateTime createdAt;
    
    private LocalDateTime updatedAt;
    
}