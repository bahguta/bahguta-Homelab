package com.bahguta.bahguta_api.dashboard.dto;

import java.time.Instant;
import java.time.LocalDate;
import java.time.ZoneOffset;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.stream.Collectors;

import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.ForecastDay;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.ForecastHour;
import com.bahguta.bahguta_api.weather.dto.Weather5diasResponse;

/**
 * Convierte los ~40 tramos de 3 h de /weather/5dias en N dias (min, max, icono, lluvia).
 * Reduce el payload de ~16 KB a ~0.5 KB.
 *
 * Criterio del icono del dia (solo tramos diurnos, icono "xxd"):
 *   - Es de precipitacion si hay >= 2 tramos diurnos con lluvia/chubasco/tormenta/nieve
 *     o >= 3 mm acumulados en esos tramos. Una lluvia debil y aislada (o nocturna)
 *     NO convierte el dia en "lluvioso".
 *   - Si no, el icono sin precipitacion mas frecuente (empate: el mas nublado).
 */
public final class WeatherForecastAggregator {

    private static final int MIN_PRECIP_SLOTS = 2;
    private static final double MIN_PRECIP_MM = 3.0;

    /** Horas de referencia por defecto de las franjas (hora local de la ciudad). */
    public static final int[] DEFAULT_HOURS = { 9, 14, 19 };

    /**
     * Un tramo solo sirve para una franja si esta a 90 min o menos de su hora. Con tramos de
     * 3 h el maximo natural es 90 min; mas alla (hoy, cuando la hora ya paso) la franja no sale.
     */
    private static final int MAX_DISTANCE_MINUTES = 90;

    private WeatherForecastAggregator() {}

    public record Slot(
            long dt,                    // epoch segundos (UTC)
            double minTemperature,
            double maxTemperature,
            String description,
            String icon,                // ej. "10d"
            Double rain3h               // puede ser null
    ) {}

    public static List<ForecastDay> toDaily(Weather5diasResponse r, int maxDays) {
        return toDaily(r, maxDays, DEFAULT_HOURS);
    }

    /** Igual que toDaily(r, maxDays) pero con las horas de referencia de las franjas (hora local). */
    public static List<ForecastDay> toDaily(Weather5diasResponse r, int maxDays, int[] hours) {
        List<Slot> slots = r.getForecast().stream()
                .filter(f -> f.getDt() != null)
                .map(f -> new Slot(
                        f.getDt(),
                        f.getMinTemperature() != null ? f.getMinTemperature() : f.getTemperature(),
                        f.getMaxTemperature() != null ? f.getMaxTemperature() : f.getTemperature(),
                        f.getDescription(),
                        f.getIcon(),
                        f.getRain3h()))
                .toList();
        int tz = r.getTimezone() == null ? 0 : r.getTimezone().intValue();
        return toDaily(slots, tz, maxDays, hours);
    }

    /**
     * @param timezoneSeconds offset de la ciudad (campo "timezone" del JSON, ej. 7200)
     * @param maxDays         normalmente 5; el primer dia puede ser parcial (solo tramos futuros)
     */
    public static List<ForecastDay> toDaily(List<Slot> slots, int timezoneSeconds, int maxDays) {
        return toDaily(slots, timezoneSeconds, maxDays, DEFAULT_HOURS);
    }

    public static List<ForecastDay> toDaily(List<Slot> slots, int timezoneSeconds, int maxDays, int[] hours) {
        ZoneOffset offset = ZoneOffset.ofTotalSeconds(timezoneSeconds);

        // Agrupa por fecha LOCAL (dtTxt viene en UTC, por eso no se usa)
        Map<LocalDate, List<Slot>> byDay = new TreeMap<>();
        for (Slot s : slots) {
            LocalDate d = Instant.ofEpochSecond(s.dt()).atOffset(offset).toLocalDate();
            byDay.computeIfAbsent(d, k -> new ArrayList<>()).add(s);
        }

        List<ForecastDay> out = new ArrayList<>();
        for (Map.Entry<LocalDate, List<Slot>> e : byDay.entrySet()) {
            if (out.size() >= maxDays) break;
            List<Slot> day = e.getValue();

            double min = day.stream().mapToDouble(Slot::minTemperature).min().orElse(0);
            double max = day.stream().mapToDouble(Slot::maxTemperature).max().orElse(0);
            double rain = day.stream().mapToDouble(WeatherForecastAggregator::rainOf).sum();
            Slot rep = representative(day);

            out.add(new ForecastDay(
                    e.getKey().toString(),
                    round1(min),
                    round1(max),
                    prefix(rep.icon()) + "d",
                    rep.description(),
                    round1(rain),
                    hourlySlots(day, offset, hours)));
        }
        return out;
    }

    /**
     * Franjas del dia para la pantalla del tiempo, una por cada hora de referencia
     * (por defecto 9:00, 14:00 y 19:00). Cada franja usa la medicion (tramo de 3 h) MAS
     * CERCANA a su hora: de ella salen la temperatura (minima/maxima del tramo) y el icono.
     * Empate de distancia: el tramo mas temprano.
     *
     * Si el tramo mas cercano esta a mas de 90 min (hoy, cuando la hora ya paso y solo quedan
     * tramos futuros) la franja no sale, asi que puede haber menos franjas que horas.
     */
    private static List<ForecastHour> hourlySlots(List<Slot> day, ZoneOffset offset, int[] hours) {
        List<ForecastHour> out = new ArrayList<>();
        if (day.isEmpty() || hours == null) return out;

        for (int ref : hours) {
            int refMinutes = ref * 60;

            Slot nearest = day.stream()
                    .filter(s -> s.icon() != null)
                    .min(Comparator.<Slot>comparingInt(s -> Math.abs(localMinutes(s, offset) - refMinutes))
                            .thenComparingInt(s -> localMinutes(s, offset)))
                    .orElse(null);

            if (nearest == null) continue;
            if (Math.abs(localMinutes(nearest, offset) - refMinutes) > MAX_DISTANCE_MINUTES) continue;

            out.add(new ForecastHour(ref, round1(nearest.minTemperature()), round1(nearest.maxTemperature()), nearest.icon()));
        }
        return out;
    }

    private static int localMinutes(Slot s, ZoneOffset offset) {
        var t = Instant.ofEpochSecond(s.dt()).atOffset(offset);
        return t.getHour() * 60 + t.getMinute();
    }

    /** Tramo que representa el dia (ver criterio en la cabecera de la clase). */
    private static Slot representative(List<Slot> day) {
        List<Slot> pool = day.stream()
                .filter(s -> s.icon() != null && s.icon().endsWith("d"))
                .toList();
        if (pool.isEmpty()) pool = day;

        List<Slot> precip = pool.stream().filter(s -> isPrecip(s.icon())).toList();
        double precipMm = precip.stream().mapToDouble(WeatherForecastAggregator::rainOf).sum();
        boolean rainyDay = precip.size() >= MIN_PRECIP_SLOTS || precipMm >= MIN_PRECIP_MM;

        List<Slot> candidates;
        if (rainyDay) {
            candidates = precip;
        } else {
            candidates = pool.stream().filter(s -> !isPrecip(s.icon())).toList();
            if (candidates.isEmpty()) candidates = pool; // solo habia 1 tramo con lluvia debil
        }

        Map<String, Long> counts = candidates.stream()
                .collect(Collectors.groupingBy(s -> prefix(s.icon()), Collectors.counting()));

        String best = counts.entrySet().stream()
                .max(Comparator.comparingLong((Map.Entry<String, Long> en) -> en.getValue())
                        .thenComparingInt(en -> severity(en.getKey())))
                .orElseThrow()
                .getKey();

        List<Slot> withBest = candidates.stream().filter(s -> prefix(s.icon()).equals(best)).toList();

        // Con lluvia, la descripcion sale del tramo en que mas llueve
        return rainyDay
                ? withBest.stream().max(Comparator.comparingDouble(WeatherForecastAggregator::rainOf)).orElseThrow()
                : withBest.get(0);
    }

    private static boolean isPrecip(String icon) {
        String p = prefix(icon);
        return p.equals("09") || p.equals("10") || p.equals("11") || p.equals("13");
    }

    private static double rainOf(Slot s) {
        return s.rain3h() == null ? 0 : s.rain3h();
    }

    private static String prefix(String icon) {
        return icon == null || icon.length() < 2 ? "01" : icon.substring(0, 2);
    }

    /** Mayor = peor tiempo (desempata cuando hay igual numero de tramos). */
    private static int severity(String prefix) {
        return switch (prefix) {
            case "11" -> 9; // tormenta
            case "13" -> 8; // nieve
            case "10" -> 7; // lluvia
            case "09" -> 6; // chubascos
            case "50" -> 5; // niebla
            case "04" -> 4; // muy nuboso
            case "03" -> 3; // nubes dispersas
            case "02" -> 2; // algo de nubes
            default -> 1;   // despejado
        };
    }

    private static double round1(double v) {
        return Math.round(v * 10.0) / 10.0;
    }
}
