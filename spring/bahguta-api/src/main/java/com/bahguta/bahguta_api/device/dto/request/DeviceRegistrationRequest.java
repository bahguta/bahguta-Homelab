package com.bahguta.bahguta_api.device.dto.request;

import java.util.List;
import java.util.UUID;
import com.bahguta.bahguta_api.device.enums.DeviceType;
import com.bahguta.bahguta_api.sensor.dto.SensorRegistrationRequest;

import lombok.Builder;

@Builder
public record DeviceRegistrationRequest (
	UUID id,
	String name,
	String description,
	DeviceType type,
	String firmwareVersion,
	String macAddress,
	String ipAddress,
	String hardwareModel,
	String location,
	String secretKey,
	List<SensorRegistrationRequest> sensors
	){}