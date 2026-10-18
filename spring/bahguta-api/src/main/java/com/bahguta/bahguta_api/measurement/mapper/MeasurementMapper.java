package com.bahguta.bahguta_api.measurement.mapper;

import java.util.List;
import org.mapstruct.Mapper;
import com.bahguta.bahguta_api.device.mapper.DeviceConfigMapper;
import com.bahguta.bahguta_api.device.mapper.DeviceMapper;
import com.bahguta.bahguta_api.measurement.dto.MeasurementDto;
import com.bahguta.bahguta_api.measurement.entity.Measurement;
import com.bahguta.bahguta_api.sensor.mapper.SensorConfigMapper;
import com.bahguta.bahguta_api.sensor.mapper.SensorMapper;

@Mapper(
		componentModel = "spring",
		uses = SensorMapper.class, 
		imports = { SensorConfigMapper.class, DeviceMapper.class, DeviceConfigMapper.class }
		)
public interface MeasurementMapper {

    MeasurementDto toDto(Measurement measurement);

    Measurement toEntity(MeasurementDto measurementDto);
    
    List<Measurement> toEntityList(List<MeasurementDto> measurementsDto);

    List<MeasurementDto> toDtoList(List<Measurement> measurements) ;
    
}