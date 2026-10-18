package com.bahguta.bahguta_api.dashboard.controller;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.bahguta.bahguta_api.common.api.ApiResponse;
import com.bahguta.bahguta_api.common.api.ApiResponses;
import com.bahguta.bahguta_api.dashboard.dto.DashboardDto;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse;
import com.bahguta.bahguta_api.dashboard.service.DashboardAssembler;
import com.bahguta.bahguta_api.dashboard.service.DashboardService;

import lombok.RequiredArgsConstructor;

@RestController
@RequestMapping("/api/v1/dashboard")
@RequiredArgsConstructor
public class DashboardController {

    private final DashboardService dashboardService;
    private final DashboardAssembler assembler;


    @GetMapping("/latest")
    public ApiResponse<DashboardDto> getDashboardLatest() {
        return ApiResponses.ok(dashboardService.getDashboardLatestMeasurements());
    }
    
    @GetMapping("/all")
    public ApiResponse<DashboardDto> getDashboardAll() {
        return ApiResponses.ok(dashboardService.getDashboardAllMeasurements());
    }
    
    @GetMapping("/data")
    public DashboardResponse getData(@RequestParam(required = false) Integer cfgVer) {
    	return assembler.getData(cfgVer);
    }

}