package com.bahguta.bahguta_api.measurement.dto.response;

import java.util.UUID;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@AllArgsConstructor
@NoArgsConstructor
@Builder
@Getter
@Setter
public class MeasurementRegisterResponse {
	
    private UUID id;
    private boolean success;

}
