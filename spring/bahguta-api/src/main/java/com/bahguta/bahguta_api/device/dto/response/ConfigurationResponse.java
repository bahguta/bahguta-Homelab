package com.bahguta.bahguta_api.device.dto.response;


import com.bahguta.bahguta_api.device.dto.DeviceConfigDto;

import lombok.Builder;

@Builder
public record ConfigurationResponse(
		DeviceConfigDto config
		) {}
