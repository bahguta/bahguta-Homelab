package com.bahguta.bahguta_api.device.service;

import java.util.List;
import java.util.UUID;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.device.dto.DeviceDto;
import com.bahguta.bahguta_api.device.dto.request.DeviceRegistrationRequest;
import com.bahguta.bahguta_api.device.dto.request.HeartbeatRequest;
import com.bahguta.bahguta_api.device.dto.response.DeviceRegistrationResponse;
import com.bahguta.bahguta_api.device.dto.response.DeviceResponse;
import com.bahguta.bahguta_api.device.dto.response.HeartbeatResponse;
import com.bahguta.bahguta_api.device.entity.Device;

public interface DeviceService {

	DeviceRegistrationResponse register(DeviceRegistrationRequest request)  throws DataNotFoundException;
	
    public List<DeviceDto> findAllWithConfig();
        
    public DeviceResponse buildDeviceResponse(Device device);
    
    public HeartbeatResponse heartbeat(HeartbeatRequest request) throws Exception;

    public DeviceDto findByIdWithConfig(UUID deviceId);
        
}