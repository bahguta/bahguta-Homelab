package com.bahguta.bahguta_api.sensor.service;


import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.device.dto.SensorConfigDto;
import com.bahguta.bahguta_api.device.entity.Device;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;
import com.bahguta.bahguta_api.sensor.dto.SensorRegistrationRequest;
import com.bahguta.bahguta_api.sensor.entity.Sensor;
import com.bahguta.bahguta_api.sensor.entity.SensorConfig;
import com.bahguta.bahguta_api.sensor.mapper.SensorConfigMapper;
import com.bahguta.bahguta_api.sensor.mapper.SensorMapper;
import com.bahguta.bahguta_api.sensor.repository.SensorRepository;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.stream.Collectors;

@Service
@RequiredArgsConstructor
@Transactional
public class SensorServiceImpl implements SensorService {

    private final SensorRepository sensorRepository;
    private final SensorMapper sensorMapper;
    private final SensorConfigService sensorConfigService;
    private final SensorConfigMapper sensorConfigMapper;

//    @Override
//    @Transactional
//    public List<SensorDto> synchronize(Device device, List<SensorRegistrationRequest> requests) {
//
//        List<Sensor> currentSensors = sensorRepository.findByDeviceId(device.getId());
//        
//        Map<String, Sensor> currentMap = new HashMap<>();
//
//        currentSensors.forEach(sensor ->  currentMap.put(sensor.getName(), sensor));
//
//        for (SensorRegistrationRequest request : requests) {
//
//            Sensor sensor = currentMap.remove(request.name());
//
//            if (sensor == null) {
//            	SensorConfig sensorConfig = sensorConfigService.getDefaultConfig();
//                sensor = Sensor.builder()
//            		.id(request.id())
//	                .deviceId(device.getId())
//	                .name(request.name())
//	                .type(request.type())
//	                .enabled(true)
//	                .sensorConfig(sensorConfig)
//	                .build();
//            } else {
//                sensor.setType(request.type());
//                sensor.setEnabled(true);
//            }
//
//            sensorRepository.save(sensor);
//        }
//
//        currentMap.values().forEach(sensor -> {
//            sensor.setEnabled(false);
//            sensorRepository.save(sensor);
//        });
//
//        currentSensors = sensorRepository.findByDeviceId(device.getId());
//        
//        return sensorMapper.toDtoList(currentSensors);
//    }
//    
    
    @Override
    @Transactional
    public List<SensorDto> createSensors(List<SensorRegistrationRequest> requests, Device device) throws DataNotFoundException {

    	List<Sensor> sensors = requests.stream().map(request -> {
    		
    		SensorConfig sensorConfig = sensorConfigService.getDefaultConfig();

        	sensorConfigService.save(sensorConfig);
        	
    		Sensor sensor = Sensor.builder()
    				.id(request.id())
    				.deviceId(device.getId())
    				.enabled(true)
    				.name(request.name())
    				.type(request.type())
    				.sensorConfig(sensorConfig)
    				.createdAt(LocalDateTime.now())
    				.updatedAt(LocalDateTime.now())
    				.build();
    		
    		return sensorRepository.save(sensor);
    		
    	}).collect(Collectors.toList());
    	
    	return sensorMapper.toDtoList(sensors);
    }
    

    @Override
    @Transactional(readOnly = true)
    public SensorDto findByDeviceIdAndName(UUID deviceId, String sensorName) throws DataNotFoundException {
        return sensorMapper.toDto(sensorRepository.findByDeviceIdAndName(deviceId, sensorName).orElseThrow(() -> 
        	new DataNotFoundException("Sensor '%s' not found for device '%s'".formatted(sensorName, deviceId))));
    }

    @Override
    @Transactional(readOnly = true)
    public List<SensorDto> findByDeviceId(UUID deviceId) {
        return sensorMapper.toDtoList(sensorRepository.findByDeviceId(deviceId));
    }
    
    @Override
    @Transactional(readOnly = true)
    public SensorDto findByIdWithConfig(UUID id){
        return sensorMapper.toDto(sensorRepository.findByIdWithConfig(id));
    }


	@Override
	public SensorDto update(SensorDto sensorDto) throws DataNotFoundException {
		Sensor sensor = sensorRepository.findById(sensorDto.getId())
				.orElseThrow(() -> new DataNotFoundException("No se ha podido encontrar el sensor con id %s".formatted(sensorDto.getId())));

		sensor.setName(sensorDto.getName());
		sensor.setUpdatedAt(LocalDateTime.now());
		sensor.setEnabled(sensorDto.isEnabled());
		sensor.setSensorConfig(sensorConfigMapper.toEntity(sensorDto.getSensorConfig()));		
		
		return sensorMapper.toDto(sensorRepository.save(sensor));
	}
	
	@Override
    public List<SensorDto> findAllWithConfig() {
        return sensorMapper.toDtoList(sensorRepository.findAllWithConfig());
    }


	@Override
	public List<SensorDto> saveAll(List<SensorDto> sensors) {
		return sensorMapper.toDtoList(sensorRepository.saveAll(sensorMapper.toEntityList(sensors)));
		
	}


	@Override
	public List<SensorDto> findByDeviceIdWithConfig(UUID deviceId) {
		return sensorMapper.toDtoList(sensorRepository.findByDeviceIdWithConfig(deviceId));
	}
	

}