package com.bahguta.bahguta_api.device.service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;

import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.common.util.Constants;
import com.bahguta.bahguta_api.config.BahgutaProperties;
import com.bahguta.bahguta_api.device.dto.DeviceConfigDto;
import com.bahguta.bahguta_api.device.dto.HeartbeatDto;
import com.bahguta.bahguta_api.device.dto.MqttDto;
import com.bahguta.bahguta_api.device.dto.request.DeviceRegistrationRequest;
import com.bahguta.bahguta_api.device.entity.DeviceConfig;
import com.bahguta.bahguta_api.device.entity.Heartbeat;
import com.bahguta.bahguta_api.device.entity.Mqtt;
import com.bahguta.bahguta_api.device.mapper.DeviceConfigMapper;
import com.bahguta.bahguta_api.device.repository.DeviceConfigRepository;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Service
@RequiredArgsConstructor
public class DeviceConfigServiceImpl implements DeviceConfigService {
		
	private final DeviceConfigRepository repository;
	private final DeviceConfigMapper mapper;
	private final BahgutaProperties properties;

	@Override
	public DeviceConfigDto findByIdDto(UUID id) throws DataNotFoundException {
		return mapper.toDto(repository.findById(id)
				.orElseThrow(() -> new DataNotFoundException("No se ha podido encontrar device config con id %s".formatted(id))));
	}
	
	@Override
	public DeviceConfig findById(UUID id) throws DataNotFoundException {
		return repository.findById(id)
				.orElseThrow(() -> new DataNotFoundException("No se ha podido encontrar device config con id %s".formatted(id)));
	}

	@Override
	public List<DeviceConfigDto> findByListOfIds(List<UUID> uuids) {
		return mapper.toDtoList(repository.findByIdIn(uuids));
	}

	@Override
	public DeviceConfigDto save(DeviceConfig deviceConfig) {
		return mapper.toDto(repository.save(deviceConfig));
	}

	@Override
	public DeviceConfig getDefaultConfig(DeviceRegistrationRequest request){
		return repository.save(DeviceConfig.builder()
				.id(request.id())
				.version(Constants.DEFAULT_VERSION)
				.firmwareVersion(request.firmwareVersion())
				.ipAddress(request.ipAddress())
				.macAddress(request.macAddress())
				.mqtt(Mqtt.builder()
						.host(properties.getMqtt().host)
						.port(properties.getMqtt().port)
						.topic(properties.getMqtt().topic)
						.build())
				.heartbeat(Heartbeat.builder()
						.success(Boolean.TRUE)
						.heartbeatSeconds(Constants.DEFAULT_HEARTBEAT_SECCONDS)
						.build())
				.createdAt(LocalDateTime.now())
				.updatedAt(LocalDateTime.now())
				.build());
		
		
		
	}

	@Override
	public DeviceConfigDto getDefaultConfigDto(DeviceRegistrationRequest request){
		return mapper.toDto(repository.save(mapper.toEntity(DeviceConfigDto.builder()
				.id(request.id())
				.version(Constants.DEFAULT_VERSION)
				.firmwareVersion(request.firmwareVersion())
				.ipAddress(request.ipAddress())
				.macAddress(request.macAddress())
				.mqtt(MqttDto.builder()
						.host(properties.getMqtt().host)
						.port(properties.getMqtt().port)
						.topic(properties.getMqtt().topic)
						.build())
				.heartbeat(HeartbeatDto.builder()
						.success(Boolean.TRUE)
						.heartbeatSeconds(Constants.DEFAULT_HEARTBEAT_SECCONDS)
						.build())
				.createdAt(LocalDateTime.now())
				.updatedAt(LocalDateTime.now())
				.build())));
	}


}
