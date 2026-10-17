package com.bahguta.bahguta_api.device.entity;

import jakarta.persistence.Embeddable;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@Embeddable
@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class Heartbeat {
	@Builder.Default
	private boolean success = true;
	private Integer heartbeatSeconds;
}
