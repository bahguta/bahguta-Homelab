package com.bahguta.bahguta_api.device.service;

import java.util.List;
import java.util.UUID;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.device.dto.DeviceConfigDto;
import com.bahguta.bahguta_api.device.dto.request.DeviceRegistrationRequest;
import com.bahguta.bahguta_api.device.entity.DeviceConfig;

public interface DeviceConfigService {
	
	DeviceConfig findById(UUID device_id) throws DataNotFoundException;
	DeviceConfigDto findByIdDto(UUID device_id) throws DataNotFoundException;
	List<DeviceConfigDto> findByListOfIds(List<UUID> uuids);
	DeviceConfigDto save(DeviceConfig deviceConfig);
	DeviceConfig getDefaultConfig(DeviceRegistrationRequest request);
	DeviceConfigDto getDefaultConfigDto(DeviceRegistrationRequest request);
	

}
