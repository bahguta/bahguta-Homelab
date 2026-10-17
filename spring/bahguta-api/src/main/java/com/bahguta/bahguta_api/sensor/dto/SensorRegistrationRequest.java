package com.bahguta.bahguta_api.sensor.dto;

import java.util.UUID;
import com.bahguta.bahguta_api.device.dto.SensorConfigDto;
import com.bahguta.bahguta_api.sensor.entity.SensorType;
import lombok.Builder;

@Builder
public record SensorRegistrationRequest(
		UUID id,
		UUID deviceId,
	    String name,
	    SensorType type,
	    SensorConfigDto sensorConfig
	    ) {}
