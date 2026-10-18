package com.bahguta.bahguta_api.sensor.service;

import java.util.List;
import java.util.UUID;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.device.dto.SensorConfigDto;
import com.bahguta.bahguta_api.sensor.entity.SensorConfig;

public interface SensorConfigService {

	SensorConfig findById(UUID sensor_id) throws DataNotFoundException;
	SensorConfigDto findByIdDto(UUID sensor_id) throws DataNotFoundException;
	List<SensorConfigDto> findByListOfIds(List<UUID> sensor_uuids);
	SensorConfigDto save(SensorConfig sensorConfig);
	SensorConfig getDefaultConfig();
	SensorConfigDto getDefaultConfigDto();
}
