package com.bahguta.bahguta_api.device.controller;

import java.util.List;
import java.util.UUID;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import com.bahguta.bahguta_api.common.api.ApiResponse;
import com.bahguta.bahguta_api.common.api.ApiResponses;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.device.dto.DeviceDto;
import com.bahguta.bahguta_api.device.dto.request.DeviceRegistrationRequest;
import com.bahguta.bahguta_api.device.dto.request.HeartbeatRequest;
import com.bahguta.bahguta_api.device.dto.response.DeviceRegistrationResponse;
import com.bahguta.bahguta_api.device.dto.response.HeartbeatResponse;
import com.bahguta.bahguta_api.device.service.DeviceService;
import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import jakarta.validation.Valid;
import lombok.RequiredArgsConstructor;

@RestController
@RequestMapping("/api/v1/devices")
@RequiredArgsConstructor
@Tag(name = "Devices", description = "Device management API")
public class DeviceController {

    private final DeviceService service;
    
    
    @PostMapping("/register")
    public ResponseEntity<DeviceRegistrationResponse> register(@Valid @RequestBody DeviceRegistrationRequest request) throws DataNotFoundException {
        return ResponseEntity.ok(service.register(request));
    }
    
    @PostMapping("/heartbeat")
    public ResponseEntity<HeartbeatResponse> heartbeat(@Valid @RequestBody HeartbeatRequest request) throws Exception {
        return ResponseEntity.ok(service.heartbeat(request));
    }
    
    @Operation(
            summary = "Get all devices",
            description = "Returns all registered devices."
    )
    @GetMapping("/list")
    public ResponseEntity<ApiResponse<List<DeviceDto>>> getAll() {
        return ResponseEntity.ok(ApiResponses.ok(service.findAllWithConfig()));
    }
    
    
    @Operation(
            summary = "Get a device by deviceId",
            description = "Returns a registered device."
    )
    @GetMapping("/device/{deviceId}")
    public ResponseEntity<ApiResponse<DeviceDto>> getDeviceByDeviceId(@PathVariable  UUID deviceId) {
        return ResponseEntity.ok(ApiResponses.ok(service.findByIdWithConfig(deviceId)));

    }

    @GetMapping("/error")
    public void error() {
        throw new RuntimeException("Esto es una prueba");
    }
}
