package com.bahguta.bahguta_api.device.dto.request;

import java.util.UUID;

import lombok.Builder;

@Builder
public record HeartbeatRequest(
		UUID deviceId,
		String firmwareVersion,
		String ipAddress,
		String macAddress,
		Integer rssi,
	    Long freeHeap,
	    Long uptime,
	    Long configVersion
		) {}
