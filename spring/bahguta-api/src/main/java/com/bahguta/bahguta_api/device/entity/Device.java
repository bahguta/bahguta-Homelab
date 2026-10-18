package com.bahguta.bahguta_api.device.entity;

import java.time.LocalDateTime;
import org.hibernate.annotations.JdbcTypeCode;
import org.hibernate.type.SqlTypes;
import com.bahguta.bahguta_api.common.entity.BaseEntity;
import com.bahguta.bahguta_api.device.enums.DeviceStatus;
import com.bahguta.bahguta_api.device.enums.DeviceType;
import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.EnumType;
import jakarta.persistence.Enumerated;
import jakarta.persistence.FetchType;
import jakarta.persistence.JoinColumn;
import jakarta.persistence.OneToOne;
import jakarta.persistence.Table;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.experimental.SuperBuilder;
import lombok.AllArgsConstructor;

@Entity
@Table(name = "device")
@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@SuperBuilder
public class Device extends BaseEntity {
	
    @Column(nullable = false, length = 100)
    private String name;

    @Column(length = 500)
    private String description;

    @Column(name = "hardware_model", length = 100)
    private String hardwareModel;
    
    @Column(name = "location", length = 100)
    private String location;

    @Enumerated(EnumType.STRING)
    @JdbcTypeCode(SqlTypes.NAMED_ENUM)
    @Column(nullable = false, length = 20)
    private DeviceStatus status;

    @Column(name = "enabled", nullable = false)
    private Boolean enabled;

    @Column(name = "secret_key", length = 64)
    private String secretKey;

    @Column(name = "last_seen")
    private LocalDateTime lastSeen;
    
    @Enumerated(EnumType.STRING)
    @JdbcTypeCode(SqlTypes.NAMED_ENUM)
    @Column(name="device_type", nullable=false)
    private DeviceType deviceType;
    
    @OneToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "device_config_id", nullable = false)
    private DeviceConfig deviceConfig;

}