package com.bahguta.bahguta_api.device.repository;

import java.util.List;
import java.util.UUID;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
import com.bahguta.bahguta_api.device.entity.DeviceConfig;

@Repository
public interface DeviceConfigRepository extends JpaRepository<DeviceConfig, UUID>{
	
	List<DeviceConfig> findByIdIn(List<UUID> ids);
	
	DeviceConfig findByVersion(long version);
	
}
