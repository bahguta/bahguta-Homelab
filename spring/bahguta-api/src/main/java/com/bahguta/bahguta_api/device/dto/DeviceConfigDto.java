package com.bahguta.bahguta_api.device.dto;

import lombok.Builder;
import lombok.Data;
import java.time.LocalDateTime;
import java.util.UUID;


@Data
@Builder
public class DeviceConfigDto {

	private UUID id;
    private long version;
    private String firmwareVersion;
    private String ipAddress;
    private String macAddress;
    private MqttDto mqtt;
    private HeartbeatDto heartbeat;
    @Builder.Default
    private LocalDateTime updatedAt = LocalDateTime.now();
    @Builder.Default
    private LocalDateTime createdAt = LocalDateTime.now();
    

}