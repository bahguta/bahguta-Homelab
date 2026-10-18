package com.bahguta.bahguta_api.device.dto;

import java.time.LocalDateTime;
import java.util.UUID;
import com.bahguta.bahguta_api.device.enums.DeviceStatus;
import com.bahguta.bahguta_api.device.enums.DeviceType;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
public class DeviceDto {

	private UUID id;
    private String name;
    private String description;
    private String hardwareModel;
    private String location;
    private DeviceStatus status;
    private Boolean enabled;
    private String secretKey;
    private LocalDateTime lastSeen;
    private DeviceType deviceType;
    private LocalDateTime createdAt;
    private LocalDateTime updatedAt;
    private DeviceConfigDto deviceConfig;
    
    
    
}
