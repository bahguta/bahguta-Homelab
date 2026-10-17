package com.bahguta.bahguta_api.sensor.service;

import java.util.List;
import java.util.UUID;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.device.entity.Device;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;
import com.bahguta.bahguta_api.sensor.dto.SensorRegistrationRequest;

public interface SensorService {

//	List<SensorDto> synchronize(Device device, List<SensorRegistrationRequest> sensors);

    SensorDto findByDeviceIdAndName(UUID deviceId, String sensorName) throws DataNotFoundException;

    List<SensorDto> findByDeviceId(UUID deviceId);
    
    List<SensorDto> findByDeviceIdWithConfig(UUID deviceId);
    
    List<SensorDto> createSensors(List<SensorRegistrationRequest> requests, Device device) throws DataNotFoundException ;
    
    SensorDto findByIdWithConfig(UUID id);
    
    SensorDto update(SensorDto sensorDto) throws DataNotFoundException;
    
    List<SensorDto> findAllWithConfig();
    
    List<SensorDto> saveAll(List<SensorDto> sensors);

}
