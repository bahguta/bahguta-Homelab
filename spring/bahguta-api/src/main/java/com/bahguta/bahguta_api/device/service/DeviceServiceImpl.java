package com.bahguta.bahguta_api.device.service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;
import java.util.UUID;
import java.util.stream.Collectors;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.common.util.Constants;
import com.bahguta.bahguta_api.device.dto.DeviceConfigDto;
import com.bahguta.bahguta_api.device.dto.DeviceDto;
import com.bahguta.bahguta_api.device.dto.HeartbeatDto;
import com.bahguta.bahguta_api.device.dto.request.DeviceRegistrationRequest;
import com.bahguta.bahguta_api.device.dto.request.HeartbeatRequest;
import com.bahguta.bahguta_api.device.dto.response.DeviceRegistrationResponse;
import com.bahguta.bahguta_api.device.dto.response.DeviceResponse;
import com.bahguta.bahguta_api.device.dto.response.HeartbeatResponse;
import com.bahguta.bahguta_api.device.entity.Device;
import com.bahguta.bahguta_api.device.entity.DeviceConfig;
import com.bahguta.bahguta_api.device.enums.DeviceStatus;
import com.bahguta.bahguta_api.device.mapper.DeviceConfigMapper;
import com.bahguta.bahguta_api.device.mapper.DeviceMapper;
import com.bahguta.bahguta_api.device.repository.DeviceRepository;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;
import com.bahguta.bahguta_api.sensor.mapper.SensorMapper;
import com.bahguta.bahguta_api.sensor.service.SensorService;
import com.bahguta.bahguta_api.system.settings.dto.SettingsData;
import com.bahguta.bahguta_api.system.settings.service.SettingsService;
import jakarta.transaction.Transactional;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Service
@RequiredArgsConstructor
public class DeviceServiceImpl implements DeviceService {
	
	private final DeviceRepository repository;
    private final DeviceMapper mapper;
    private final SensorService sensorService;
    private final DeviceConfigService deviceConfigService;
    private final DeviceConfigMapper deviceConfigMapper;
    private final SensorMapper sensorMapper;
    private final SettingsService settingsService;
    
    
    
    @Scheduled(fixedDelay = 60000)
	public void updateOfflineDevices() throws DataNotFoundException {

	    List<Device> onlineDevices = repository.findByStatus(DeviceStatus.ONLINE);

	    for (Device device : onlineDevices) {
	    	DeviceConfigDto deviceConfig = deviceConfigService.findByIdDto(device.getId());
	    	
	        if (device.getLastSeen().isBefore(LocalDateTime.now().minusSeconds(deviceConfig.getHeartbeat().heartbeatSeconds() * 2L))) {
	            device.setStatus(DeviceStatus.OFFLINE);
	        }
	    }

	    repository.saveAll(onlineDevices);
	}
    
    
    
    
    @Override
    @Transactional
    public DeviceRegistrationResponse register(DeviceRegistrationRequest request) throws DataNotFoundException {
    	
        Optional<Device> opt = repository.findById(request.id());
        
        if (!opt.isPresent()) {
        	DeviceConfig deviceConfig = deviceConfigService.getDefaultConfig(request);
        	
        	Device device = createDevice(request, deviceConfig);
        	
        	List<SensorDto> sensors = sensorService.createSensors(request.sensors(), device);
             
            return DeviceRegistrationResponse.builder()
                 .deviceId(device.getId())
                 .name(device.getName())
                 .registered(true)
                 .config(deviceConfigMapper.toDto(deviceConfig))
                 .sensors(sensors)
                 .build();
        } else {
        	
        	Device device = opt.get();
        	
        	DeviceConfigDto deviceConfig = deviceConfigMapper.toDto(device.getDeviceConfig());
            deviceConfig.setMacAddress(request.macAddress());
            deviceConfig.setIpAddress(request.ipAddress());
            deviceConfig.setFirmwareVersion(request.firmwareVersion());
        	
        	List<SensorDto> sensors = sensorService.findByDeviceIdWithConfig(device.getId());

        	return DeviceRegistrationResponse.builder()
                    .deviceId(device.getId())
                    .name(device.getName())
                    .registered(true)
                    .config(deviceConfig)
                    .sensors(sensors)
                    .build();
        }
    }
    
    

    @Override
    public List<DeviceDto> findAllWithConfig() {
        return mapper.toDtoList(repository.findAllWithConfig());
    }
    
    
    @Override
    public DeviceResponse buildDeviceResponse(Device device) {
    	List<SensorDto> sensors = sensorService.findByDeviceIdWithConfig(device.getId());
        return DeviceResponse.builder()
                .deviceId(device.getId())
                .name(device.getName())
                .description(device.getDescription())
                .hardwareModel(device.getHardwareModel())
                .location(device.getLocation())
                .status(device.getStatus())
                .deviceType(device.getDeviceType())
                .enabled(device.getEnabled())
                .lastSeen(device.getLastSeen())
                .createdAt(device.getCreatedAt())
                .updatedAt(device.getUpdatedAt())
                .secretKey(device.getSecretKey())
                .sensors(sensors)
                .build();

    }

    @Override
   	public DeviceDto findByIdWithConfig(UUID deviceId) {
   		return mapper.toDto(repository.findByIdWithConfig(deviceId));
   	}
    
    @Override
    public HeartbeatResponse heartbeat(HeartbeatRequest request) throws Exception{
    	
    	Device device = repository.findByIdWithConfig(request.deviceId());
    	
    	if (null == device) {
    		return HeartbeatResponse.builder()
    				.success(Boolean.FALSE)
    				.config(null)
    				.message("No se ha podido encontrar el device con %s".formatted(request.deviceId()))
    				.build();
    	}

    	DeviceConfigDto deviceConfig = deviceConfigService.findByIdDto(device.getDeviceConfig().getId());
    	
    	if (null == deviceConfig) {
    		return HeartbeatResponse.builder()
    				.success(Boolean.FALSE)
    				.config(null)
    				.message("No se ha podido encontrar el deviceConfig con %s".formatted(device.getDeviceConfig().getId()))
    				.build();
    	}
    	
    	/**
    	 * TODO: Hay que hacer algo con la informacion que viene.
    	 * 
    	 
     	UUID deviceId,
		String firmwareVersion,
		String ip,
		Integer rssi,
	    Long freeHeap,
	    Long uptime,
	    Long configVersion
	    
    	 */
    	
    	/*
    	 * Intervalo de medicion: el esp32c3 duerme heartbeat.heartbeatSeconds
    	 * (lo guarda en NVS y lo usa en deep sleep; sensorConfig.interval solo
    	 * lo guarda, no lo usa). El valor que se edita desde la tablet vive en
    	 * app_settings.sensor_interval_seconds, asi que aqui manda sobre los dos.
    	 */
    	Integer measurementSeconds = measurementSeconds();
    	
    	if (measurementSeconds != null) {
    		boolean success = deviceConfig.getHeartbeat() == null || deviceConfig.getHeartbeat().success();
    		deviceConfig.setHeartbeat(new HeartbeatDto(success, measurementSeconds));
    	}
    	
    	deviceConfig.setFirmwareVersion(request.firmwareVersion());
    	deviceConfig.setIpAddress(request.ipAddress());
    	deviceConfig.setMacAddress(request.macAddress());
    	
	    deviceConfig = deviceConfigService.save(deviceConfigMapper.toEntity(deviceConfig));
	    
	    List<SensorDto> sensors = sensorService.findByDeviceIdWithConfig(device.getId());
	    
	    if (measurementSeconds != null) {
	    	for (SensorDto sensor : sensors) {
	    		if (sensor.getSensorConfig() != null) {
	    			sensor.getSensorConfig().setInterval(measurementSeconds);
	    		}
	    	}
	    }
	    

    	device.setLastSeen(LocalDateTime.now());
		device.setStatus(DeviceStatus.ONLINE);
    	
    	repository.save(device);
    	
    	return HeartbeatResponse.builder()
    			.success(Boolean.TRUE)
    			.config(deviceConfig)
    			.sensors(sensors)
    			.build();
    }

    
    /** Intervalo de medicion global (app_settings); null si no se puede leer. */
    private Integer measurementSeconds() {
    	try {
    		SettingsData settings = settingsService.getSettings();
    		return settings == null ? Constants.DEFAULT_HEARTBEAT_SECCONDS : settings.apiRefreshSeconds();
    	} catch (Exception e) {
    		log.warn("No se pudo leer app_settings; se mantiene el heartbeat del device: {}", e.getMessage());
    		return null;
    	}
    }

    private Device createDevice(DeviceRegistrationRequest request, DeviceConfig config) throws DataNotFoundException {
    	
        Device device = Device.builder()
        		.id(request.id())
                .name(request.name())
                .description(request.description())
                .hardwareModel(request.hardwareModel())
                .location(request.location())
                .status(DeviceStatus.ONLINE)
                .enabled(true)
                .secretKey(request.secretKey())
                .lastSeen(LocalDateTime.now())
                .deviceType(request.type())
                .deviceConfig(config)
                .createdAt(LocalDateTime.now())
                .updatedAt(LocalDateTime.now())
                .build();

        return repository.save(device);
    }

    
}
