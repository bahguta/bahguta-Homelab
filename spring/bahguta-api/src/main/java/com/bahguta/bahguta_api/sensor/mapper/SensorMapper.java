package com.bahguta.bahguta_api.sensor.mapper;

import com.bahguta.bahguta_api.sensor.dto.SensorDto;
import com.bahguta.bahguta_api.sensor.entity.Sensor;
import java.util.List;
import org.mapstruct.Mapper;

@Mapper(
	    componentModel = "spring",
	    uses = SensorConfigMapper.class
	)
public interface SensorMapper {
	
	SensorDto toDto(Sensor sensor);
	
	Sensor toEntity(SensorDto sensorDto);
	
	List<Sensor> toEntityList(List<SensorDto> sensorsDto);
	
	List<SensorDto> toDtoList(List<Sensor> sensors);

}