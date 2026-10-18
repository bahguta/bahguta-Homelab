package com.bahguta.bahguta_api.sensor.controller;

import com.bahguta.bahguta_api.common.api.ApiResponse;
import com.bahguta.bahguta_api.common.api.ApiResponses;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;
import com.bahguta.bahguta_api.sensor.service.SensorService;
import lombok.RequiredArgsConstructor;
import org.springframework.web.bind.annotation.*;
import java.util.List;
import java.util.UUID;

@RestController
@RequestMapping("/api/v1/sensors")
@RequiredArgsConstructor
public class SensorController {

    private final SensorService sensorService;
    
    
    @GetMapping("/list")
    public ApiResponse<List<SensorDto>> getAll() throws DataNotFoundException {
        return ApiResponses.ok(sensorService.findAllWithConfig());
    }

    @GetMapping("/device/{deviceId}")
    public ApiResponse<List<SensorDto>> findByDevice(@PathVariable UUID deviceId) {
        return ApiResponses.ok(sensorService.findByDeviceIdWithConfig(deviceId));
    }

    @GetMapping("/sensor/{sensorId}")
    public ApiResponse<SensorDto> get(@PathVariable UUID sensorId) throws DataNotFoundException {
        return ApiResponses.ok(sensorService.findByIdWithConfig(sensorId));
    }

    @PutMapping("/sensor/{sensorId}")
    public ApiResponse<SensorDto> update(@PathVariable UUID sensorId, @RequestBody SensorDto request) throws DataNotFoundException {
        return ApiResponses.ok(sensorService.update(request));
    }

}
