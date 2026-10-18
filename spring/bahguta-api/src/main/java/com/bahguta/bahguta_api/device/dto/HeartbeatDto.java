package com.bahguta.bahguta_api.device.dto;

import lombok.Builder;

@Builder
public record HeartbeatDto(
		boolean success,
		Integer heartbeatSeconds
		) {}