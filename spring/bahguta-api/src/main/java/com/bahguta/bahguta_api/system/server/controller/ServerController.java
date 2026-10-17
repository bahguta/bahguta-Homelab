package com.bahguta.bahguta_api.system.server.controller;


import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import com.bahguta.bahguta_api.common.api.ApiResponse;
import com.bahguta.bahguta_api.common.api.ApiResponses;
import com.bahguta.bahguta_api.system.server.dto.BackendServerDto;
import com.bahguta.bahguta_api.system.server.service.ServerService;

import lombok.RequiredArgsConstructor;

@RestController
@RequestMapping("/api/v1/system/server")
@RequiredArgsConstructor
public class ServerController {

    private final ServerService ServerService;

    @GetMapping
    public ApiResponse<BackendServerDto> getMetrics() {
        return ApiResponses.ok(ServerService.getMetrics());
    }
}