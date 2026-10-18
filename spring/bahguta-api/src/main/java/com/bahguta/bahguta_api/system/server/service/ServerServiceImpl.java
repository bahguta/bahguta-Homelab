package com.bahguta.bahguta_api.system.server.service;


import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.system.server.dto.BackendServerDto;
import com.bahguta.bahguta_api.system.server.queries.PrometheusQueries;

import lombok.RequiredArgsConstructor;

@Service
@RequiredArgsConstructor
public class ServerServiceImpl implements ServerService {

    private final PrometheusClient prometheusClient;

    @Override
    public BackendServerDto getMetrics()
    {
        double cpu =
            prometheusClient.query(
                PrometheusQueries.CPU
            );

        double ram =
            prometheusClient.query(
                PrometheusQueries.RAM
            );
        
        double uptime = 
        	prometheusClient.query(
    			PrometheusQueries.UPTIME);

        return new BackendServerDto(
            cpu,
            ram,
            uptime
        );
    }
}
