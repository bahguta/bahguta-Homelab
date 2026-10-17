package com.bahguta.bahguta_api.device.entity;

import com.bahguta.bahguta_api.common.entity.BaseEntity;
import jakarta.persistence.AttributeOverride;
import jakarta.persistence.AttributeOverrides;
import jakarta.persistence.Column;
import jakarta.persistence.Embedded;
import jakarta.persistence.Entity;
import jakarta.persistence.Table;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.experimental.SuperBuilder;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@SuperBuilder
@Entity
@Table(name = "device_config")
public class DeviceConfig extends BaseEntity {
	
	@Column(nullable = false)
	private long version;
	
	@Column(name = "firmware_version")
	private String firmwareVersion;
	
	@Column(name = "ip_address")
	private String ipAddress;
	
	@Column(name = "mac_address")
    private String macAddress;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "host", column = @Column(name ="mqtt_host")),
        @AttributeOverride(name = "port", column = @Column(name ="mqtt_port")),
        @AttributeOverride(name = "topic", column = @Column(name ="mqtt_topic")
        )
    })
    private Mqtt mqtt;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "heartbeatSeconds",column = @Column(name = "heartbeat_seconds")),
        @AttributeOverride(name = "success",column = @Column(name = "success")
        )
    })
    private Heartbeat heartbeat;
    
    
}
