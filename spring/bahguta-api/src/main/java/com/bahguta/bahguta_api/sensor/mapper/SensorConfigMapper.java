package com.bahguta.bahguta_api.sensor.mapper;

import java.util.List;
import org.mapstruct.Mapper;
import com.bahguta.bahguta_api.device.dto.SensorConfigDto;
import com.bahguta.bahguta_api.sensor.entity.SensorConfig;

@Mapper(componentModel = "spring")
public interface SensorConfigMapper {

	SensorConfigDto toDto(SensorConfig entity);

	SensorConfig toEntity(SensorConfigDto dto);
    
    List<SensorConfigDto> toDtoList(List<SensorConfig> entities);
    
    List<SensorConfig> toEntityList(List<SensorConfigDto> entities);
}
