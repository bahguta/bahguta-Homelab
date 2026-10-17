package com.bahguta.bahguta_api.system.server.dto;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class BackendServerDto {

    private double cpu;
    private double ram;
    private double uptime;
}
