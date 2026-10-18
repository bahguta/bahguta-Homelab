package com.bahguta.bahguta_api.measurement.dto;

import com.bahguta.bahguta_api.measurement.dto.request.MeasurementRequest;
import com.bahguta.bahguta_api.measurement.entity.Measurement;
import com.bahguta.bahguta_api.sensor.entity.Sensor;

public interface MeasurementFactory {

    Measurement create(Sensor sensor, MeasurementRequest request);

}