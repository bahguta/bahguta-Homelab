package com.bahguta.bahguta_api.system.server.service;

import java.util.Map;

/** Una serie de Prometheus: sus etiquetas (metric) y su valor actual. */
public record PromSample(Map<String, String> labels, double value) {}
