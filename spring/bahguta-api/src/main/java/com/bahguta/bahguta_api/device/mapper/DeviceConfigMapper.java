package com.bahguta.bahguta_api.device.mapper;

import java.util.List;
import org.mapstruct.Mapper;
import com.bahguta.bahguta_api.device.dto.DeviceConfigDto;
import com.bahguta.bahguta_api.device.entity.DeviceConfig;

@Mapper(componentModel = "spring")
public interface DeviceConfigMapper {
	
	DeviceConfigDto toDto(DeviceConfig entity);

	DeviceConfig toEntity(DeviceConfigDto dto);
    
    List<DeviceConfigDto> toDtoList(List<DeviceConfig> entities);

}
