package com.bahguta.bahguta_api.sensor.entity;

import com.bahguta.bahguta_api.common.entity.BaseEntity;
import jakarta.persistence.Column;
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
@Table(name = "sensor_config")
public class SensorConfig extends BaseEntity {
    
    @Column(nullable = false)
    private long version;

    @Column(nullable=false)
    private boolean enabled;

    @Column(nullable=false)
    private Integer interval;

}
