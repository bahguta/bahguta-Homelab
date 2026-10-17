package com.bahguta.bahguta_api.device.dto.response;


import java.util.List;

import com.bahguta.bahguta_api.device.dto.DeviceConfigDto;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;

import lombok.Builder;

@Builder
public record HeartbeatResponse(
		boolean success,
		DeviceConfigDto config,
		List<SensorDto> sensors,
		String message
		) {}
