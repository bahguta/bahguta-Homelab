package com.bahguta.bahguta_api.measurement.service;

import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.ZoneOffset;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.function.Function;
import java.util.stream.Collectors;

import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.device.entity.Device;
import com.bahguta.bahguta_api.device.repository.DeviceRepository;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementSummaryResponse;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementSummaryResponse.Point;
import com.bahguta.bahguta_api.measurement.dto.response.MeasurementSummaryResponse.RoomSeries;
import com.bahguta.bahguta_api.measurement.repository.MeasurementRepository;
import com.bahguta.bahguta_api.measurement.repository.MeasurementSummaryRow;
import com.bahguta.bahguta_api.sensor.entity.Sensor;
import com.bahguta.bahguta_api.sensor.repository.SensorRepository;

import lombok.RequiredArgsConstructor;

/**
 * Resumen del historico para la grafica de CasaScreen: una unica consulta agrupada en base
 * de datos y una respuesta de pocos KB (frente a ~1 MB por sensor del endpoint /history).
 */
@Service
@RequiredArgsConstructor
public class MeasurementSummaryService {

    private static final int MIN_HOURS = 1, MAX_HOURS = 168;
    private static final int MIN_POINTS = 6, MAX_POINTS = 96;

    private final MeasurementRepository measurementRepository;
    private final SensorRepository sensorRepository;
    private final DeviceRepository deviceRepository;

    public MeasurementSummaryResponse summary(int hours, int points) {
        hours = Math.max(MIN_HOURS, Math.min(MAX_HOURS, hours));
        points = Math.max(MIN_POINTS, Math.min(MAX_POINTS, points));

        final int step = Math.max(60, hours * 3600 / points);

        // created_at se guarda como LocalDateTime de la zona del servidor
        final ZoneId zone = ZoneId.systemDefault();
        final LocalDateTime to = LocalDateTime.now();
        final LocalDateTime from = to.minusHours(hours);

        // sensor -> habitacion (ubicacion del device); solo sensores activos
        List<Sensor> sensors = sensorRepository.findAll().stream().filter(Sensor::isEnabled).toList();
        Map<UUID, Device> devices = deviceRepository
                .findAllById(sensors.stream().map(Sensor::getDeviceId).distinct().toList())
                .stream().collect(Collectors.toMap(Device::getId, Function.identity()));

        Map<String, List<Point>> pointsBySensor = new LinkedHashMap<>();
        for (MeasurementSummaryRow row : measurementRepository.summarize(from, to, step)) {
            long ts = LocalDateTime.ofEpochSecond(row.getBucket(), 0, ZoneOffset.UTC)
                    .atZone(zone).toEpochSecond();
            pointsBySensor.computeIfAbsent(row.getSensorId(), k -> new ArrayList<>())
                    .add(new Point(ts, round1(row.getTemperature()), round1(row.getHumidity())));
        }

        List<RoomSeries> rooms = new ArrayList<>();
        for (Sensor s : sensors) {
            Device d = devices.get(s.getDeviceId());
            String room = (d != null && d.getLocation() != null && !d.getLocation().isBlank())
                    ? d.getLocation() : (d != null ? d.getName() : s.getName());

            rooms.add(new RoomSeries(room, s.getId().toString(),
                    pointsBySensor.getOrDefault(s.getId().toString(), List.of())));
        }
        rooms.sort((a, b) -> a.room().compareToIgnoreCase(b.room()));

        return new MeasurementSummaryResponse(
                from.atZone(zone).toEpochSecond(), to.atZone(zone).toEpochSecond(), step, rooms);
    }

    private static Double round1(Double v) {
        return v == null ? null : Math.round(v * 10d) / 10d;
    }
}
