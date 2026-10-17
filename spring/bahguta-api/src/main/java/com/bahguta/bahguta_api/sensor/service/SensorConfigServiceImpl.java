package com.bahguta.bahguta_api.sensor.service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;
import org.springframework.stereotype.Service;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.common.util.Constants;
import com.bahguta.bahguta_api.device.dto.SensorConfigDto;
import com.bahguta.bahguta_api.sensor.entity.SensorConfig;
import com.bahguta.bahguta_api.sensor.mapper.SensorConfigMapper;
import com.bahguta.bahguta_api.sensor.repository.SensorConfigRepository;

import lombok.RequiredArgsConstructor;

@Service
@RequiredArgsConstructor
public class SensorConfigServiceImpl implements SensorConfigService {
	
	private final SensorConfigMapper mapper;
	private final SensorConfigRepository repo;
	
	
	@Override
	public SensorConfig findById(UUID sensor_id) throws DataNotFoundException {
		return repo.findById(sensor_id)
				.orElseThrow(() -> new DataNotFoundException("No se ha encontrado configuracion para sensor_config_id %s".formatted(sensor_id)));
	}

	@Override
	public SensorConfigDto findByIdDto(UUID sensor_id) throws DataNotFoundException {
		return mapper.toDto(repo.findById(sensor_id)
				.orElseThrow(() -> new DataNotFoundException("No se ha encontrado configuracion para sensor_config_id %s".formatted(sensor_id))));
	}

	@Override
	public List<SensorConfigDto> findByListOfIds(List<UUID> sensor_uuids) {
		return mapper.toDtoList(repo.findByIdIn(sensor_uuids));
	}

	@Override
	public SensorConfigDto save(SensorConfig sensorConfig) {
		return mapper.toDto(repo.save(sensorConfig));
	}
	
	@Override
	public SensorConfig getDefaultConfig() {
		return SensorConfig.builder()
		        .id(UUID.randomUUID())
		        .version(Constants.DEFAULT_VERSION)
		        .enabled(Boolean.TRUE)
		        .interval(Constants.DEFAULT_HEARTBEAT_SECCONDS)
		        .createdAt(LocalDateTime.now())
		        .updatedAt(LocalDateTime.now())
		        .build();
	}

	@Override
	public SensorConfigDto getDefaultConfigDto() {
		return SensorConfigDto.builder()
		        .id(UUID.randomUUID())
		        .version(Constants.DEFAULT_VERSION)
		        .enabled(Boolean.TRUE)
		        .interval(Constants.DEFAULT_HEARTBEAT_SECCONDS)
		        .createdAt(LocalDateTime.now())
		        .updatedAt(LocalDateTime.now())
		        .build();
	}


}
