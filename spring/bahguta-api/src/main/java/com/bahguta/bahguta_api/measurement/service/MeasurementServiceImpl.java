package com.bahguta.bahguta_api.measurement.service;


import com.bahguta.bahguta_api.measurement.dto.request.MeasurementRequest;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementRegisterResponse;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementResponse;
import com.bahguta.bahguta_api.measurement.entity.Measurement;
import com.bahguta.bahguta_api.measurement.mapper.MeasurementMapper;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.measurement.dto.MeasurementDto;
import com.bahguta.bahguta_api.measurement.dto.MeasurementFactory;
import com.bahguta.bahguta_api.measurement.repository.MeasurementRepository;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;
import com.bahguta.bahguta_api.sensor.mapper.SensorMapper;
import com.bahguta.bahguta_api.sensor.service.SensorService;
import lombok.RequiredArgsConstructor;
import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

@Service
@RequiredArgsConstructor
@Transactional
public class MeasurementServiceImpl implements MeasurementService {

    private final MeasurementRepository measurementRepository;
    private final SensorService sensorService;
    private final SensorMapper sensorMapper;
    private final MeasurementFactory measurementFactory;
    private final MeasurementMapper mapper;

    @Override
    public MeasurementRegisterResponse registerMeasurement(MeasurementRequest request) throws DataNotFoundException {

        SensorDto sensor = sensorService.findByIdWithConfig(request.getSensorId());
        
        if (sensor == null ) {
        	throw new DataNotFoundException("No se ha podido encontrar un sensor con el id %s".formatted(request.getSensorId()));
        }

        Measurement measurement = measurementFactory.create(sensorMapper.toEntity(sensor), request);

        measurementRepository.save(measurement);
        
        return MeasurementRegisterResponse.builder()
        		.id(measurement.getId())
        		.success(true)
        		.build();
    }

	@Override
	public MeasurementResponse getLatest(UUID sensorId) {
		return MeasurementResponse.builder()
				.measurements(mapper.toDtoList(measurementRepository.findTop100BySensorIdOrderByCreatedAtDesc(sensorId)))
				.build(); 
	}


	@Override
	public MeasurementResponse history(UUID sensorId, LocalDateTime from, LocalDateTime to) {
		return MeasurementResponse.builder()
				.measurements(mapper.toDtoList(measurementRepository.findBySensorIdAndCreatedAtBetweenOrderByCreatedAtAsc(sensorId, from, to)))
				.build(); 
	}

	@Override
	public MeasurementDto getById(UUID measurementId) throws DataNotFoundException {
		return mapper.toDto(measurementRepository.findById(measurementId)
				.orElseThrow(() -> new DataNotFoundException("No se ha podido encontrar un measurement con el id %s".formatted(measurementId))));
	}

	@Override
	public List<MeasurementDto> getLatestDto(UUID sensorId) {
		return mapper.toDtoList(measurementRepository.findTop100BySensorIdOrderByCreatedAtDesc(sensorId));
	}

	@Override
	public List<MeasurementDto> historyDto(UUID sensorId, LocalDateTime from, LocalDateTime to) {
		return mapper.toDtoList(measurementRepository.findBySensorIdAndCreatedAtBetweenOrderByCreatedAtAsc(sensorId, from, to));
	}
	
	@Override
    public List<MeasurementDto> findAll() {
        return mapper.toDtoList(measurementRepository.findAll());
    }

	@Override
	public List<MeasurementDto> findLatestMeasurements() {
		return mapper.toDtoList(measurementRepository.findLatestMeasurements());
	}

}
