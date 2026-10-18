package com.bahguta.bahguta_api.common.exception;

import java.time.LocalDateTime;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;
import org.springframework.web.server.ResponseStatusException;

import com.bahguta.bahguta_api.common.util.Constants;

import lombok.extern.slf4j.Slf4j;

@Slf4j
@RestControllerAdvice
public class GlobalExceptionHandler {

    @ExceptionHandler(Exception.class)
    public ResponseEntity<ApiErrorResponse> handleException(Exception ex) {

        ApiErrorResponse response = ApiErrorResponse.builder()
                .success(false)
                .timestamp(LocalDateTime.now())
                .message(Constants.INTERNAL_SERVER_ERROR)
                .build();
        
        log.error(ex.getMessage(), ex);

        return ResponseEntity
                .status(HttpStatus.INTERNAL_SERVER_ERROR)
                .body(response);
    }
    
    @ExceptionHandler(DataNotFoundException.class)
    public ResponseEntity<ApiErrorResponse> handleDataNotFoundException(Exception ex){
    	ApiErrorResponse response = ApiErrorResponse.builder()
                .success(false)
                .timestamp(LocalDateTime.now())
                .message(Constants.ERROR_MSG_NOT_FOUND)
                .build();
        
        log.error(ex.getMessage(), ex);

        return ResponseEntity.status(HttpStatus.NOT_FOUND).body(response);
    }

    @ExceptionHandler(ResponseStatusException.class)
    public ResponseEntity<ApiErrorResponse> handleResponseStatusException(Exception ex){
    	ApiErrorResponse response = ApiErrorResponse.builder()
                .success(false)
                .timestamp(LocalDateTime.now())
                .message(Constants.ERROR_MSG_NOT_FOUND)
                .build();
        
        log.error(ex.getMessage(), ex);

        return ResponseEntity.status(HttpStatus.NOT_FOUND).body(response);
    }

}