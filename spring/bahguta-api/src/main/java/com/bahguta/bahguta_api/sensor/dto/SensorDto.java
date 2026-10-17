package com.bahguta.bahguta_api.sensor.dto;

import com.bahguta.bahguta_api.device.dto.SensorConfigDto;
import com.bahguta.bahguta_api.sensor.entity.SensorType;
import lombok.*;
import java.time.LocalDateTime;
import java.util.UUID;

@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
public class SensorDto {

    private UUID id;
    private UUID deviceId;
    private String name;
    private SensorType type;
    private boolean enabled;
    private LocalDateTime createdAt;
    private LocalDateTime updatedAt;
    private SensorConfigDto sensorConfig;

}