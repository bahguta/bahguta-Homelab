package com.bahguta.bahguta_api.measurement.dto;

import com.bahguta.bahguta_api.measurement.dto.request.MeasurementRequest;
import com.bahguta.bahguta_api.measurement.entity.Measurement;
import com.bahguta.bahguta_api.sensor.entity.Sensor;

import lombok.RequiredArgsConstructor;

import java.time.LocalDateTime;
import java.util.UUID;

import org.springframework.stereotype.Component;

@Component
@RequiredArgsConstructor
public class DefaultMeasurementFactory implements MeasurementFactory {

    @Override
    public Measurement create(Sensor sensor, MeasurementRequest request) {
        return Measurement.builder()
        		.id(UUID.randomUUID())
                .sensor(sensor)
                .temperature(request.getTemperature())
                .humidity(request.getHumidity())
                .pressure(request.getPressure())
                .valor(request.getValue())
                .createdAt(LocalDateTime.now())
                .updatedAt(LocalDateTime.now())
                .build();
    }

}
