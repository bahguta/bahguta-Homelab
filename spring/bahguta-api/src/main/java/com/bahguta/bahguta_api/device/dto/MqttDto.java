package com.bahguta.bahguta_api.device.dto;

import lombok.Builder;

@Builder
public record MqttDto(
		String host,
		Integer port,
		String topic
		) {}
