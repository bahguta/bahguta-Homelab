package com.bahguta.bahguta_api.measurement.controller;


import com.bahguta.bahguta_api.common.api.ApiResponse;
import com.bahguta.bahguta_api.common.api.ApiResponses;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.measurement.dto.request.MeasurementRequest;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementRegisterResponse;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementResponse;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementSummaryResponse;
import com.bahguta.bahguta_api.measurement.service.MeasurementService;
import com.bahguta.bahguta_api.measurement.service.MeasurementSummaryService;
import io.swagger.v3.oas.annotations.Operation;
import jakarta.validation.Valid;
import lombok.RequiredArgsConstructor;

import java.time.LocalDateTime;
import java.util.UUID;

import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api/v1/measurements")
@RequiredArgsConstructor
public class MeasurementController {

    private final MeasurementService measurementService;
    private final MeasurementSummaryService measurementSummaryService;

    @PostMapping("/register")
    @Operation(summary = "Registers a new measurement")
    public ApiResponse<MeasurementRegisterResponse> registerMeasurement(@Valid @RequestBody MeasurementRequest request) throws DataNotFoundException {
        return ApiResponses.ok(measurementService.registerMeasurement(request));
    }
    
    
    @GetMapping("/{sensorId}/latest")
    @Operation(summary = "Get latests measurements")
    public ApiResponse<MeasurementResponse> latestMeasurements(@PathVariable UUID sensorId) throws DataNotFoundException {
        return ApiResponses.ok(measurementService.getLatest(sensorId));
    }

    @GetMapping("/{sensorId}/history")
    @Operation(summary = "Get history of measurements between 2 dates")
    public ApiResponse<MeasurementResponse> historyMeasurements(@PathVariable UUID sensorId, @RequestParam LocalDateTime from, @RequestParam LocalDateTime to) throws DataNotFoundException {
        return ApiResponses.ok(measurementService.history(sensorId, from, to));
    }

    @GetMapping("/summary")
    @Operation(summary = "Compact history for the tablet chart: per-room averages by time bucket")
    public ApiResponse<MeasurementSummaryResponse> summary(
            @RequestParam(defaultValue = "24") int hours,
            @RequestParam(defaultValue = "24") int points) {
        return ApiResponses.ok(measurementSummaryService.summary(hours, points));
    }

}
