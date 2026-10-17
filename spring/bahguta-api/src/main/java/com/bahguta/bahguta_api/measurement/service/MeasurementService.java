package com.bahguta.bahguta_api.measurement.service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.measurement.dto.MeasurementDto;
import com.bahguta.bahguta_api.measurement.dto.request.MeasurementRequest;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementRegisterResponse;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementResponse;

public interface MeasurementService {

	MeasurementRegisterResponse registerMeasurement(MeasurementRequest request) throws DataNotFoundException;
    
	MeasurementResponse getLatest(UUID sensorId);
	
	List<MeasurementDto> getLatestDto(UUID sensorId);

	MeasurementResponse history(UUID sensorId, LocalDateTime from, LocalDateTime to);
	
	List<MeasurementDto> historyDto(UUID sensorId, LocalDateTime from, LocalDateTime to);
    
    MeasurementDto getById(UUID measurementId) throws DataNotFoundException;
    
    List<MeasurementDto> findAll();
    
    List<MeasurementDto> findLatestMeasurements();

}
