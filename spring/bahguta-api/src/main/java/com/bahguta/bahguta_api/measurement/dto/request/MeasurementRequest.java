package com.bahguta.bahguta_api.measurement.dto.request;

import lombok.Builder;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import lombok.*;

import java.util.UUID;

@Getter
@Setter
@Builder
@NoArgsConstructor
@AllArgsConstructor
public class MeasurementRequest {

    @NotNull
    private UUID sensorId;

    @NotBlank
    private String sensorName;

    private Double temperature;

    private Double humidity;

    private Double pressure;

    private Double value;

}