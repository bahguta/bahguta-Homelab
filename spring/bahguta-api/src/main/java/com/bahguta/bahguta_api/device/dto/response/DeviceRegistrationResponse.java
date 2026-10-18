package com.bahguta.bahguta_api.device.dto.response;

import java.util.List;
import java.util.UUID;
import com.bahguta.bahguta_api.device.dto.DeviceConfigDto;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;
import com.bahguta.bahguta_api.sensor.entity.Sensor;

import lombok.Builder;

@Builder
public record DeviceRegistrationResponse(
        UUID deviceId,
        String name,
        boolean registered,
        DeviceConfigDto config,
        List<SensorDto> sensors
        ) {}