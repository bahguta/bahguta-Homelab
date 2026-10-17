package com.bahguta.bahguta_api.system.docker.service;

import java.nio.file.FileSystemException;
import java.util.List;

import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.common.exception.FailCommandExecuteException;
import com.bahguta.bahguta_api.common.exception.FileNotFoundException;
import com.bahguta.bahguta_api.system.docker.dto.DockerContainerDto;
import com.bahguta.bahguta_api.system.executor.ScriptExecutor;

import lombok.RequiredArgsConstructor;

@Service
@RequiredArgsConstructor
public class DockerServiceImpl implements DockerService {
	
	private final ScriptExecutor scriptExecutor;

	@Override
	public List<DockerContainerDto> getContainers() throws FileNotFoundException, FailCommandExecuteException, FileSystemException {
		return scriptExecutor.getContainerList();
	}
	
	@Override
    public void startContainer(String containerId) throws FileSystemException, FileNotFoundException, FailCommandExecuteException {
		scriptExecutor.startContainer(containerId);
    }

    @Override
    public void stopContainer(String containerId) throws FileSystemException, FileNotFoundException, FailCommandExecuteException {
    	scriptExecutor.stopContainer(containerId);
    }


}
