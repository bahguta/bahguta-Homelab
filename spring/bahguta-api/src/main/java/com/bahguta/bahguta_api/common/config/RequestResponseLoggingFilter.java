package com.bahguta.bahguta_api.common.config;


import jakarta.servlet.FilterChain;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Component;
import org.springframework.web.filter.OncePerRequestFilter;
import org.springframework.web.util.ContentCachingRequestWrapper;
import org.springframework.web.util.ContentCachingResponseWrapper;

import java.io.IOException;
import java.io.UnsupportedEncodingException;

@Component
public class RequestResponseLoggingFilter extends OncePerRequestFilter {

    private static final Logger log = LoggerFactory.getLogger(RequestResponseLoggingFilter.class);

    @Override
    protected void doFilterInternal(HttpServletRequest request, HttpServletResponse response, FilterChain filterChain)
            throws ServletException, IOException {
        
        // Excluir rutas de actuadores, métricas o estáticos si lo deseas
        String path = request.getRequestURI();
        if (path.contains("/actuator") || path.contains("/swagger") || path.contains("/v3/api-docs")) {
            filterChain.doFilter(request, response);
            return;
        }

        // Envolvemos el request y response para poder leer sus cuerpos (body) varias veces
        ContentCachingRequestWrapper requestWrapper = new ContentCachingRequestWrapper(request);
        ContentCachingResponseWrapper responseWrapper = new ContentCachingResponseWrapper(response);

        long startTime = System.currentTimeMillis();

        try {
            // Continúa la ejecución hacia el Controller
            filterChain.doFilter(requestWrapper, responseWrapper);
        } finally  {
            long duration = System.currentTimeMillis() - startTime;
            logRequest(requestWrapper);
            logResponse(responseWrapper, duration);
            
            // CRÍTICAL: Copia el cuerpo de la respuesta de vuelta al stream original de salida
            responseWrapper.copyBodyToResponse();
        }
    }

    private void logRequest(ContentCachingRequestWrapper request) {
        String body = getStringValue(request.getContentAsByteArray(), request.getCharacterEncoding());
        log.info("========== HTTP INBOUND ==========");
        log.info("Method      : {}", request.getMethod());
        log.info("URI         : {}{}", request.getRequestURI(), 
                request.getQueryString() != null ? "?" + request.getQueryString() : "");
        log.info("Client IP   : {}", request.getRemoteAddr());
        if (!body.isEmpty()) {
            log.info("Payload     : {}", body.trim());
        }
        log.info("==================================");
    }

    private void logResponse(ContentCachingResponseWrapper response, long duration) {
        String body = getStringValue(response.getContentAsByteArray(), response.getCharacterEncoding());
        log.info("========== HTTP OUTBOUND =========");
        log.info("Status      : {}", response.getStatus());
        log.info("Duration    : {} ms", duration);
        if (!body.isEmpty()) {
            log.info("Response    : {}", body.trim());
        }
        log.info("==================================");
    }

    private String getStringValue(byte[] contentAsByteArray, String characterEncoding) {
        try {
            return new String(contentAsByteArray, characterEncoding != null ? characterEncoding : "UTF-8");
        } catch (UnsupportedEncodingException e) {
            return "[Error al decodificar el cuerpo]";
        }
    }
}
