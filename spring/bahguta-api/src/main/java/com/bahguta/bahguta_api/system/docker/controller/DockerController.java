package com.bahguta.bahguta_api.system.docker.controller;

import java.nio.file.FileSystemException;
import java.util.List;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import com.bahguta.bahguta_api.common.api.ApiResponse;
import com.bahguta.bahguta_api.common.api.ApiResponses;
import com.bahguta.bahguta_api.common.exception.DataNotFoundException;
import com.bahguta.bahguta_api.common.exception.FailCommandExecuteException;
import com.bahguta.bahguta_api.common.exception.FileNotFoundException;
import com.bahguta.bahguta_api.system.docker.dto.DockerContainerDto;
import com.bahguta.bahguta_api.system.docker.service.DockerService;
import io.swagger.v3.oas.annotations.Operation;
import lombok.RequiredArgsConstructor;

@RestController
@RequestMapping("/api/v1/system/docker")
@RequiredArgsConstructor
public class DockerController {
	
	private final DockerService dockerService;
	
	@GetMapping("/container/list")
    @Operation(summary = "Get all contenedors")
    public ApiResponse<List<DockerContainerDto>> getAllContainers() throws DataNotFoundException, FileNotFoundException, FailCommandExecuteException, FileSystemException {
        return ApiResponses.ok(dockerService.getContainers());
    }
	
	@PostMapping("/container/{id}/start")
	@Operation(summary = "Start a container")
	public ApiResponse<Void> startContainer(@PathVariable String id) throws FileSystemException, FileNotFoundException, FailCommandExecuteException {
	    dockerService.startContainer(id);
	    return ApiResponses.ok(null);
	}

	@PostMapping("/container/{id}/stop")
	@Operation(summary = "Stop a container")
	public ApiResponse<Void> stopContainer(@PathVariable String id) throws FileSystemException, FileNotFoundException, FailCommandExecuteException {
	    dockerService.stopContainer(id);
	    return ApiResponses.ok(null);
	}

}
