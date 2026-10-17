package com.bahguta.bahguta_api.dashboard.service;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import org.springframework.stereotype.Service;
import com.bahguta.bahguta_api.dashboard.dto.DashboardDto;
import com.bahguta.bahguta_api.dashboard.dto.DashboardSensorDto;
import com.bahguta.bahguta_api.device.dto.DeviceDto;
import com.bahguta.bahguta_api.device.service.DeviceService;
import com.bahguta.bahguta_api.measurement.dto.MeasurementDto;
import com.bahguta.bahguta_api.measurement.service.MeasurementService;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;
import com.bahguta.bahguta_api.sensor.service.SensorService;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Service
@RequiredArgsConstructor
public class DashboardServiceImpl implements DashboardService {
	
	private final DeviceService deviceService;
	private final SensorService sensorService;
	private final MeasurementService measurementService;

	@Override
	public DashboardDto getDashboardLatestMeasurements() {
		return getDashboard(measurementService.findLatestMeasurements());
	}
	
	@Override
	public DashboardDto getDashboardAllMeasurements() {
		return getDashboard(measurementService.findAll());
	}
	
	
	private DashboardDto getDashboard(List<MeasurementDto> measurements) {
		
		List<DeviceDto> devices = deviceService.findAllWithConfig();
		List<SensorDto> sensors = sensorService.findAllWithConfig();
		
		int totalDevices = devices.size();
		int onlineDevices = devices.stream().filter(d->d.getEnabled()).collect(Collectors.toList()).size();
		int offlineDevices = totalDevices - onlineDevices;
				
		int totalSensors = sensors.size();
		int onlineSensors = sensors.stream().filter(s->s.isEnabled()).collect(Collectors.toList()).size();
		int offlineSensors = totalSensors - onlineSensors;
				
		List<DashboardSensorDto> listaDashboardsSensorDto = new ArrayList<DashboardSensorDto>();
		
		measurements.stream().forEach(measurement -> {
			Optional<SensorDto> optSensor = sensors.stream().filter(s -> s.getId().equals(measurement.getSensor().getId())).findFirst();
			
			if(optSensor.isPresent()) {
				SensorDto sensorDto = optSensor.get();
				Optional<DeviceDto> optDevice = devices.stream().filter(d -> d.getId().equals(sensorDto.getDeviceId())).findFirst();
				
				if (optDevice.isPresent()) {
					DeviceDto deviceDto = optDevice.get();
					
					listaDashboardsSensorDto.add(DashboardSensorDto.builder()
						.deviceId(deviceDto.getId())
						.deviceName(deviceDto.getName())
						.sensorId(sensorDto.getId())
						.sensorName(sensorDto.getName())
						.sensorType(sensorDto.getType())
						.enabled(sensorDto.isEnabled())
						.temperature(measurement.getTemperature())
						.humidity(measurement.getHumidity())
						.pressure(measurement.getPressure())
						.value(measurement.getValor())
						.lastMeasurement(measurement.getCreatedAt())
						.build());
				}
			}
		});
		
		
		return DashboardDto.builder()
				.totalDevices(totalDevices)
				.onlineDevices(onlineDevices)
				.offlineDevices(offlineDevices)
				.totalSensors(totalSensors)
				.onlineSensors(onlineSensors)
				.offlineSensors(offlineSensors)
				.sensors(listaDashboardsSensorDto)
				.build();

	}

}
