package com.bahguta.bahguta_api.sensor.entity;

import java.util.UUID;

import org.hibernate.annotations.JdbcTypeCode;
import org.hibernate.type.SqlTypes;

import com.bahguta.bahguta_api.common.entity.BaseEntity;
import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.EnumType;
import jakarta.persistence.Enumerated;
import jakarta.persistence.FetchType;
import jakarta.persistence.JoinColumn;
import jakarta.persistence.OneToOne;
import jakarta.persistence.Table;
import jakarta.persistence.UniqueConstraint;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.experimental.SuperBuilder;

@Entity
@Table(
    name = "sensor",
    uniqueConstraints = {
        @UniqueConstraint(
            name = "uk_sensor_device_name",
            columnNames = {"device_id", "name"}
        )
    }
)
@Getter
@Setter
@SuperBuilder
@NoArgsConstructor
@AllArgsConstructor
public class Sensor extends BaseEntity {

	@Column(name = "device_id", nullable = false)
	private UUID deviceId;

    @Enumerated(EnumType.STRING)
    @JdbcTypeCode(SqlTypes.NAMED_ENUM)
    @Column(name = "type", nullable = false, length = 50)
    private SensorType type;

    @Column(nullable = false, length = 50)
    private String name;

    @Builder.Default
    @Column(nullable = false)
    private boolean enabled = true;
    
    @OneToOne(fetch = FetchType.LAZY)
    @JoinColumn(name="sensor_config_id")
    private SensorConfig sensorConfig;
}

