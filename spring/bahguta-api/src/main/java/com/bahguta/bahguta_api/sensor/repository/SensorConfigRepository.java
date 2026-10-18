package com.bahguta.bahguta_api.sensor.repository;

import java.util.List;
import java.util.UUID;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
import com.bahguta.bahguta_api.sensor.entity.SensorConfig;

@Repository
public interface SensorConfigRepository extends JpaRepository<SensorConfig, UUID>{

	List<SensorConfig> findByIdIn(List<UUID> ids);
}
