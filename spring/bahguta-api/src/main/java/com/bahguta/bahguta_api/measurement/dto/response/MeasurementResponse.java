package com.bahguta.bahguta_api.measurement.dto.response;

import lombok.*;
import java.util.List;
import com.bahguta.bahguta_api.measurement.dto.MeasurementDto;

@Getter
@Setter
@Builder
@NoArgsConstructor
@AllArgsConstructor
public class MeasurementResponse {

	List<MeasurementDto> measurements;

}