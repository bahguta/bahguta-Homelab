package com.bahguta.bahguta_api.measurement.repository;

/** Fila del resumen agrupado en base de datos (un tramo de un sensor). */
public interface MeasurementSummaryRow {
    String getSensorId();
    Long getBucket();          // inicio del tramo: segundos "naive" de created_at
    Double getTemperature();
    Double getHumidity();
}
