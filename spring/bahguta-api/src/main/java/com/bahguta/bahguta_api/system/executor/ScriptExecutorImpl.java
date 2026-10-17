package com.bahguta.bahguta_api.system.executor;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.file.FileSystemException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import org.springframework.dao.PermissionDeniedDataAccessException;
import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.common.exception.FailCommandExecuteException;
import com.bahguta.bahguta_api.common.exception.FileNotFoundException;
import com.bahguta.bahguta_api.config.BahgutaProperties;
import com.bahguta.bahguta_api.system.docker.dto.DockerContainerDto;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Service
@RequiredArgsConstructor
public class ScriptExecutorImpl implements ScriptExecutor {

    private final BahgutaProperties properties;
    private final ObjectMapper objectMapper;
    
    @Override
    public List<DockerContainerDto> getContainerList() throws FileSystemException, FileNotFoundException, FailCommandExecuteException {
    	String json = execute(properties.scripts.docker.list, (String[]) null);
    	
    	try {
    		return objectMapper.readValue(json,  new TypeReference<List<DockerContainerDto>>() {});
		} catch (JsonProcessingException e) {
			throw new FailCommandExecuteException("Unable to parse docker response.", e);
		}
    }
    
    @Override
    public void startContainer(String containerId) throws FileSystemException, FileNotFoundException, FailCommandExecuteException {
        execute(properties.scripts.docker.start, containerId);
    }

    @Override
    public void stopContainer(String containerId) throws FileSystemException, FileNotFoundException, FailCommandExecuteException {
        execute(properties.scripts.docker.stop, containerId);
    }

    private String execute(String script, String... args) throws FileSystemException, FileNotFoundException {

        Path scriptPath = Path.of(properties.scripts.path, script);
                        
        if (!Files.exists(scriptPath)) {
            throw new FileNotFoundException("Script not found.");
        }

        List<String> command = new ArrayList<>();
        command.add(scriptPath.toString());

        if (args != null) {
            for (String arg : args) {
                command.add(arg);
            }
        }

        ProcessBuilder processBuilder = new ProcessBuilder(command);

        try {

            Process process = processBuilder.start();

            StringBuilder output = new StringBuilder();
            StringBuilder errorOutput = new StringBuilder();

            Thread stdoutThread = new Thread(() -> {
                try (BufferedReader reader = new BufferedReader(
                        new InputStreamReader(process.getInputStream()))) {

                    String line;

                    while ((line = reader.readLine()) != null) {
                        output.append(line).append(System.lineSeparator());
                    }

                } catch (IOException e) {
                    log.error("Error reading stdout", e);
                }
            });

            Thread stderrThread = new Thread(() -> {
                try (BufferedReader reader = new BufferedReader(
                        new InputStreamReader(process.getErrorStream()))) {

                    String line;

                    while ((line = reader.readLine()) != null) {
                        errorOutput.append(line).append(System.lineSeparator());
                    }

                } catch (IOException e) {
                    log.error("Error reading stderr", e);
                }
            });

            stdoutThread.start();
            stderrThread.start();

            int exitCode = process.waitFor();

            stdoutThread.join();
            stderrThread.join();

            if (errorOutput.length() > 0) {
                log.warn(
                    "Script '{}' stderr:\n{}",
                    script,
                    errorOutput
                );
            }

            if (exitCode != 0) {
                log.error(
                    "Script '{}' finished with exit code {}",
                    script,
                    exitCode
                );

                throw new PermissionDeniedDataAccessException(
                    "Unable to execute system command.",
                    null
                );
            }

            return output.toString();

        } catch (IOException e) {

            log.error("Error executing script '{}'", script, e);

            throw new FileSystemException(
                "Unable to execute system command."
            );

        } catch (InterruptedException e) {

            Thread.currentThread().interrupt();

            throw new FileSystemException(
                "Command execution interrupted."
            );
        }

    }

}