package com.bahguta.bahguta_api.system.docker.service;

import java.nio.file.FileSystemException;
import java.util.List;

import com.bahguta.bahguta_api.common.exception.FailCommandExecuteException;
import com.bahguta.bahguta_api.common.exception.FileNotFoundException;
import com.bahguta.bahguta_api.system.docker.dto.DockerContainerDto;

public interface DockerService {

    List<DockerContainerDto> getContainers() throws FileNotFoundException, FailCommandExecuteException, FileSystemException;
    
    void startContainer(String containerId) throws FileSystemException, FileNotFoundException, FailCommandExecuteException;
    
    void stopContainer(String containerId) throws FileSystemException, FileNotFoundException, FailCommandExecuteException;

}