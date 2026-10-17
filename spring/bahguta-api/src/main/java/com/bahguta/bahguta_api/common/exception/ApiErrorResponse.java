package com.bahguta.bahguta_api.common.exception;

import java.time.LocalDateTime;

import lombok.Builder;
import lombok.Getter;

@Getter
@Builder
public class ApiErrorResponse {

    private boolean success;
    private LocalDateTime timestamp;
    private String message;

}