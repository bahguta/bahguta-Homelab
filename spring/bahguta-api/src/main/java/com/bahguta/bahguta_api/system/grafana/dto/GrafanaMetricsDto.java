package com.bahguta.bahguta_api.system.grafana.dto;

/**
 * Metricas del servidor leidas de Prometheus (las mismas que pinta Grafana).
 * Discos en GiB (como "df -h"), CPU y RAM en %, temperatura en grados C.
 * Cualquier valor que Prometheus no devuelva va a null.
 */
public record GrafanaMetricsDto(
        Double rootTotalGb,
        Double rootUsedGb,
        Double rootFreeGb,
        Double storageTotalGb,
        Double storageUsedGb,
        Double storageFreeGb,
        Double cpu,
        Double ram,
        Double tempC
) {}
