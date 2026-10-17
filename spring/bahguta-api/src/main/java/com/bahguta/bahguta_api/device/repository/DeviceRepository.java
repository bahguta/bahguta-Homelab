package com.bahguta.bahguta_api.device.repository;

import java.util.List;
import java.util.UUID;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;
import com.bahguta.bahguta_api.device.entity.Device;
import com.bahguta.bahguta_api.device.enums.DeviceStatus;

@Repository
public interface DeviceRepository extends JpaRepository<Device, UUID> {

	List<Device> findByStatus(DeviceStatus status);
	
	@Query("""
		    select d
		    from Device d
		    left join fetch d.deviceConfig
		""")
	List<Device> findAllWithConfig();
	
	@Query("""
		    select d
		    from Device d
		    left join fetch d.deviceConfig
		    where d.id = :id
		""")
	Device findByIdWithConfig(UUID id);
}