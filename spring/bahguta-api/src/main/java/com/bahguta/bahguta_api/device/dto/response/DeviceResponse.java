package com.bahguta.bahguta_api.device.dto.response;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;

import com.bahguta.bahguta_api.device.dto.DeviceConfigDto;
import com.bahguta.bahguta_api.device.enums.DeviceStatus;
import com.bahguta.bahguta_api.device.enums.DeviceType;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;

import io.swagger.v3.oas.annotations.media.Schema;
import lombok.Builder;
import lombok.Data;

@Schema(description = "Device information")
@Data
@Builder
public class DeviceResponse {

	private UUID deviceId;
    private String name;
    private String description;
    private String macAddress;
    private String hardwareModel;
    private String firmwareVersion;
    private String ipAddress;
    private String location;
    private DeviceStatus status;
    private Boolean enabled;
    private String secretKey;
    private LocalDateTime lastSeen;
    private DeviceType deviceType;
    private LocalDateTime createdAt;
    private LocalDateTime updatedAt;
    private DeviceConfigDto deviceConfig;
    private List<SensorDto> sensors;

}