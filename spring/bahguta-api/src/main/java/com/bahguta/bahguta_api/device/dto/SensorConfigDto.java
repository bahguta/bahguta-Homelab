package com.bahguta.bahguta_api.device.dto;

import java.time.LocalDateTime;
import java.util.UUID;
import lombok.Builder;
import lombok.Data;

@Data
@Builder
public class SensorConfigDto {

	private UUID id;
	private long version;
    private Boolean enabled;
    private Integer interval;
    @Builder.Default
    private LocalDateTime updatedAt = LocalDateTime.now();;
    @Builder.Default
    private LocalDateTime createdAt = LocalDateTime.now();
}
