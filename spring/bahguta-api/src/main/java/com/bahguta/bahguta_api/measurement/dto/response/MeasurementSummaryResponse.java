package com.bahguta.bahguta_api.measurement.dto.response;

import java.util.List;

/**
 * Historico COMPACTO para la grafica de la tablet: una serie por sensor/habitacion con la
 * media de temperatura y humedad por tramo de tiempo (en vez de miles de mediciones sueltas).
 *
 * from/to/ts son epoch en segundos (UTC); stepSeconds es la duracion de cada tramo y ts el
 * inicio del tramo. Solo salen los tramos que tienen datos.
 */
public record MeasurementSummaryResponse(
        long from,
        long to,
        int stepSeconds,
        List<RoomSeries> rooms
) {
    public record RoomSeries(String room, String sensorId, List<Point> points) {}

    /** t = temperatura media (C), h = humedad media (%); null si el tramo no tiene ese dato. */
    public record Point(long ts, Double t, Double h) {}
}
