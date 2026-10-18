package com.bahguta.bahguta_api.system.grafana.service;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.config.BahgutaProperties;
import com.bahguta.bahguta_api.system.grafana.dto.GrafanaMetricsDto;
import com.bahguta.bahguta_api.system.server.queries.PrometheusQueries;
import com.bahguta.bahguta_api.system.server.service.PromSample;
import com.bahguta.bahguta_api.system.server.service.PrometheusClient;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * Metricas para la pantalla Grafana de la tablet. Cada metrica falla por
 * separado: si Prometheus no tiene la temperatura, el resto sigue llegando.
 *
 * Discos: se leen TODAS las series de node_filesystem_* y se eligen en Java.
 * Con node_exporter en Docker el mountpoint llega con prefijo (/rootfs, /host...)
 * o solo se ve el device, asi que se acepta cualquiera de los dos.
 * Todo se puede cambiar en application.properties (bahguta.grafana.*).
 */
@Slf4j
@Service
@RequiredArgsConstructor
public class GrafanaMetricsService {

    private static final double GIB = 1024d * 1024d * 1024d;
    private static final long LOG_EVERY_MS = 10 * 60 * 1000L;

    private static final String FS_FILTER = "{fstype!~\"tmpfs|overlay|squashfs|devtmpfs|ramfs\"}";
    private static final List<String> ROOT_PREFIXES =
            List.of("/rootfs", "/host/root", "/hostroot", "/hostfs", "/host");

    private static final List<String> DEFAULT_TEMP_QUERIES = List.of(
            "max(node_hwmon_temp_celsius)",
            "max(node_thermal_zone_temp)");

    private final PrometheusClient prometheus;
    private final BahgutaProperties properties;

    private final AtomicLong lastDiskLog = new AtomicLong(0);

    public GrafanaMetricsDto getMetrics() {
        BahgutaProperties.Grafana cfg = properties.getGrafana();

        String rootMount = pick(cfg == null ? null : cfg.getRootMount(), "/");
        String rootDevice = pick(cfg == null ? null : cfg.getRootDevice(), "ubuntu--lv");
        String storageMount = pick(cfg == null ? null : cfg.getStorageMount(), "/srv/storage");
        String storageDevice = pick(cfg == null ? null : cfg.getStorageDevice(), "sda1");

        List<PromSample> sizes = tryVector("node_filesystem_size_bytes" + FS_FILTER);
        List<PromSample> avails = tryVector("node_filesystem_avail_bytes" + FS_FILTER);
        List<PromSample> frees = tryVector("node_filesystem_free_bytes" + FS_FILTER);

        Double[] root = disk(sizes, avails, frees, rootMount, rootDevice);
        Double[] storage = disk(sizes, avails, frees, storageMount, storageDevice);

        if (root[0] == null || storage[0] == null) {
            logAvailableDisks(sizes);
        }

        List<String> tempQueries = new ArrayList<>();
        if (cfg != null && cfg.getTempQuery() != null && !cfg.getTempQuery().isBlank()) {
            tempQueries.add(cfg.getTempQuery());
        }
        tempQueries.addAll(DEFAULT_TEMP_QUERIES);

        Double temp = null;
        for (String q : tempQueries) {
            temp = tryQuery(q);
            if (temp != null) {
                break;
            }
        }

        return new GrafanaMetricsDto(
                root[0], root[1], root[2],
                storage[0], storage[1], storage[2],
                round1(tryQuery(PrometheusQueries.CPU)),
                round1(tryQuery(PrometheusQueries.RAM)),
                round1(temp));
    }

    /** [total, usado, libre] en GiB; todo null si no se encuentra el disco. */
    private Double[] disk(List<PromSample> sizes, List<PromSample> avails, List<PromSample> frees,
            String mount, String deviceHint) {
        PromSample size = null;

        for (PromSample s : sizes) {
            if (matches(s.labels(), mount, deviceHint) && (size == null || s.value() > size.value())) {
                size = s;
            }
        }

        if (size == null) {
            return new Double[] { null, null, null };
        }

        PromSample avail = sameFs(avails, size);
        PromSample free = sameFs(frees, size);

        double total = size.value() / GIB;

        if (avail == null) {
            return new Double[] { round1(total), null, null };
        }

        // Como "df": Libre = disponible para usuarios; Ocupado = total - libre real
        double availGb = avail.value() / GIB;
        double usedGb = total - (free != null ? free.value() / GIB : availGb);

        return new Double[] { round1(total), round1(Math.max(usedGb, 0)), round1(availGb) };
    }

    private PromSample sameFs(List<PromSample> list, PromSample ref) {
        for (PromSample s : list) {
            if (sameFilesystem(s.labels(), ref.labels())) {
                return s;
            }
        }
        return null;
    }

    private boolean matches(Map<String, String> labels, String mount, String deviceHint) {
        String device = labels.getOrDefault("device", "");

        if (normalizeMount(labels.getOrDefault("mountpoint", "")).equals(mount)) {
            return true;
        }

        return deviceHint != null && !deviceHint.isBlank() && device.contains(deviceHint);
    }

    /** /rootfs -> /, /rootfs/srv/storage -> /srv/storage, etc. */
    private String normalizeMount(String mountpoint) {
        for (String prefix : ROOT_PREFIXES) {
            if (mountpoint.equals(prefix)) {
                return "/";
            }
            if (mountpoint.startsWith(prefix + "/")) {
                return mountpoint.substring(prefix.length());
            }
        }
        return mountpoint;
    }

    private boolean sameFilesystem(Map<String, String> a, Map<String, String> b) {
        return a.getOrDefault("device", "").equals(b.getOrDefault("device", ""))
                && a.getOrDefault("mountpoint", "").equals(b.getOrDefault("mountpoint", ""));
    }

    /** Si no se encuentra un disco, deja en el log que series ve Prometheus (1 vez cada 10 min). */
    private void logAvailableDisks(List<PromSample> sizes) {
        long now = System.currentTimeMillis();
        long last = lastDiskLog.get();

        if (now - last < LOG_EVERY_MS || !lastDiskLog.compareAndSet(last, now)) {
            return;
        }

        String found = sizes.stream()
                .limit(20)
                .map(s -> s.labels().getOrDefault("mountpoint", "?") + " (" + s.labels().getOrDefault("device", "?") + ")")
                .collect(Collectors.joining(", "));

        log.warn("Grafana: disco no encontrado. Series node_filesystem_size_bytes: [{}]. "
                + "Ajusta bahguta.grafana.root-mount/root-device/storage-mount/storage-device.",
                found.isEmpty() ? "ninguna" : found);
    }

    private List<PromSample> tryVector(String query) {
        try {
            return prometheus.queryVector(query);
        } catch (RuntimeException e) {
            log.debug("Metrica sin datos ({}): {}", query, e.getMessage());
            return List.of();
        }
    }

    /** null si Prometheus no contesta, no hay datos o el valor no es un numero. */
    private Double tryQuery(String query) {
        try {
            double v = prometheus.query(query);
            return Double.isNaN(v) || Double.isInfinite(v) ? null : v;
        } catch (RuntimeException e) {
            log.debug("Metrica sin datos ({}): {}", query, e.getMessage());
            return null;
        }
    }

    private static Double round1(Double v) {
        return v == null ? null : Math.round(v * 10d) / 10d;
    }

    private static String pick(String configured, String fallback) {
        return configured == null || configured.isBlank() ? fallback : configured;
    }
}
