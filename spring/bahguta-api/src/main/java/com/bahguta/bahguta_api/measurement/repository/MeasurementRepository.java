package com.bahguta.bahguta_api.measurement.repository;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

import com.bahguta.bahguta_api.measurement.entity.Measurement;

public interface MeasurementRepository extends JpaRepository<Measurement, UUID> {

	List<Measurement> findTop100BySensorIdOrderByCreatedAtDesc(UUID sensorId);
	
	List<Measurement> findBySensorIdAndCreatedAtBetweenOrderByCreatedAtAsc(UUID sensorId, LocalDateTime from, LocalDateTime to);
	
	@Query("""
		SELECT m
		FROM Measurement m
		WHERE m.createdAt = (
		    SELECT MAX(m2.createdAt)
		    FROM Measurement m2
		    WHERE m2.sensor.id = m.sensor.id
		)
	""")
	List<Measurement> findLatestMeasurements();

	/**
	 * Media por tramos de :step segundos para todos los sensores en [from, to).
	 * Descarta lecturas imposibles del DHT22 (temperatura fuera de -40..80, humedad fuera de 0..100).
	 */
	@Query(value = """
		SELECT CAST(m.sensor_id AS varchar) AS "sensorId",
		       CAST(FLOOR(EXTRACT(EPOCH FROM m.created_at) / :step) * :step AS bigint) AS "bucket",
		       AVG(m.temperature) AS "temperature",
		       AVG(m.humidity) AS "humidity"
		FROM measurement m
		WHERE m.created_at >= :from AND m.created_at < :to
		  AND (m.temperature IS NULL OR m.temperature BETWEEN -40 AND 80)
		  AND (m.humidity IS NULL OR m.humidity BETWEEN 0 AND 100)
		GROUP BY m.sensor_id, 2
		ORDER BY m.sensor_id, 2
		""", nativeQuery = true)
	List<MeasurementSummaryRow> summarize(
			@org.springframework.data.repository.query.Param("from") LocalDateTime from,
			@org.springframework.data.repository.query.Param("to") LocalDateTime to,
			@org.springframework.data.repository.query.Param("step") long step);

}
