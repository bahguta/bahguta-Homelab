package com.bahguta.bahguta_api.system.docker.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@Builder
@AllArgsConstructor
@NoArgsConstructor
@Getter
@Setter
public class DockerContainerDto {
	
	private String id;

	private String name;

	private String image;

	private String status;

	private String state;

	private String ports;
	

}
