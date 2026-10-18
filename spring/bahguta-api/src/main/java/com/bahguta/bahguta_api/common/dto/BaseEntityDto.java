package com.bahguta.bahguta_api.common.dto;

import java.time.LocalDateTime;
import java.util.UUID;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@AllArgsConstructor
@NoArgsConstructor
@Builder
@Data
public class BaseEntityDto {

	private UUID id;
	private LocalDateTime createdAt;
	private LocalDateTime updatedAt;
}
