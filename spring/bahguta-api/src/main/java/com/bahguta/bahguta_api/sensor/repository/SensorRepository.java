package com.bahguta.bahguta_api.sensor.repository;

import java.util.List;
import java.util.Optional;
import java.util.UUID;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import com.bahguta.bahguta_api.sensor.entity.Sensor;

public interface SensorRepository extends JpaRepository<Sensor, UUID> {

    Optional<Sensor> findByDeviceIdAndName(UUID deviceId, String name);

    
    
    List<Sensor> findByDeviceId(UUID deviceId);
    
    @Query("""
    	    select s
    	    from Sensor s
    	    left join fetch s.sensorConfig
    	    where s.deviceId = :deviceId
    	""")
	List<Sensor> findByDeviceIdWithConfig(UUID deviceId);
    
    @Query("""
		    select s
		    from Sensor s
		    left join fetch s.sensorConfig
		""")
    List<Sensor> findAllWithConfig();
    
    @Query("""
		    select s
		    from Sensor s
		    left join fetch s.sensorConfig
		    where s.id = :id
		""")
    Sensor findByIdWithConfig(UUID id);

}