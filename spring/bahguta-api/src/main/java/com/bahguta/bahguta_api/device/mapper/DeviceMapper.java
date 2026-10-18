package com.bahguta.bahguta_api.device.mapper;

import java.util.List;
import org.mapstruct.Mapper;
import com.bahguta.bahguta_api.device.dto.DeviceDto;
import com.bahguta.bahguta_api.device.entity.Device;

@Mapper(
	    componentModel = "spring",
	    uses = DeviceConfigMapper.class
	)
public interface DeviceMapper {

    DeviceDto toDto(Device entity);

    Device toEntity(DeviceDto dto);
    
    List<DeviceDto> toDtoList(List<Device> entities);
    
  

}
