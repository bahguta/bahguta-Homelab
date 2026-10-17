package com.bahguta.bahguta_api.dashboard.service;

import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.UUID;
import java.util.concurrent.Callable;

import org.springframework.boot.info.BuildProperties;
import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.Counter;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.CurrentWeather;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.DockerContainer;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.ForecastDay;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.Room;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.Section;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.Sensor;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.Summary;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.SystemStats;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.TabletConfig;
import com.bahguta.bahguta_api.dashboard.dto.DashboardResponse.WeatherData;
import com.bahguta.bahguta_api.dashboard.dto.WeatherForecastAggregator;
import com.bahguta.bahguta_api.device.dto.DeviceConfigDto;
import com.bahguta.bahguta_api.device.dto.DeviceDto;
import com.bahguta.bahguta_api.device.dto.SensorConfigDto;
import com.bahguta.bahguta_api.device.enums.DeviceStatus;
import com.bahguta.bahguta_api.device.service.DeviceService;
import com.bahguta.bahguta_api.measurement.dto.MeasurementDto;
import com.bahguta.bahguta_api.measurement.service.MeasurementService;
import com.bahguta.bahguta_api.sensor.dto.SensorDto;
import com.bahguta.bahguta_api.sensor.service.SensorService;
import com.bahguta.bahguta_api.system.docker.dto.DockerContainerDto;
import com.bahguta.bahguta_api.system.docker.service.DockerService;
import com.bahguta.bahguta_api.system.server.dto.BackendServerDto;
import com.bahguta.bahguta_api.system.server.service.ServerService;
import com.bahguta.bahguta_api.system.settings.dto.SettingsData;
import com.bahguta.bahguta_api.system.settings.service.SettingsService;
import com.bahguta.bahguta_api.weather.dto.Weather5diasResponse;
import com.bahguta.bahguta_api.weather.dto.WeatherResponse;
import com.bahguta.bahguta_api.weather.service.WeatherService;
import com.bahguta.bahguta_api.system.grafana.dto.GrafanaMetricsDto;
import com.bahguta.bahguta_api.system.grafana.service.GrafanaMetricsService;
import com.bahguta.bahguta_api.system.network.dto.NetworkClientsDto;
import com.bahguta.bahguta_api.system.network.service.OmadaClientService;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Service
@RequiredArgsConstructor
public class DashboardAssembler {

	private static final int DEFAULT_INTERVAL_SECONDS = 1800;
	private static final long MIN_TOLERANCE_SECONDS = 900; // 15 min

	// IMPORTANTE: final, si no @RequiredArgsConstructor no los inyecta (NPE)
	private final DeviceService deviceService;
	private final MeasurementService measurementService;
	private final SensorService sensorService;
	private final DockerService dockerService;
	private final ServerService serverService;
	private final SettingsService settingsService;
	private final WeatherService weatherService;
	private final GrafanaMetricsService grafanaMetricsService;
	private final OmadaClientService omadaClientService;
	private final BuildProperties buildProperties;

	public DashboardResponse getData(Integer cfgVer) {
		long now = Instant.now().getEpochSecond();

		// Cada fuente falla de forma independiente (null => su seccion sale ok=false)
		List<DeviceDto> devices = tryGet("devices", deviceService::findAllWithConfig);
		List<SensorDto> sensors = tryGet("sensors", sensorService::findAllWithConfig);
		List<MeasurementDto> measurements = tryGet("measurements", measurementService::findAll);
		List<DockerContainerDto> dockerList = tryGet("docker", dockerService::getContainers);
		BackendServerDto server = tryGet("server", serverService::getMetrics);
		SettingsData settings = tryGet("settings", settingsService::getSettings);
		WeatherResponse weather = tryGet("weather", weatherService::getWeather);
		Weather5diasResponse weather5 = tryGet("weather5", weatherService::getWeather5days);
		GrafanaMetricsDto grafana = tryGet("grafana", grafanaMetricsService::getMetrics);
		NetworkClientsDto red = tryGet("red", omadaClientService::getClients);

		Map<UUID, MeasurementDto> latest = latestBySensor(measurements);

		// ---------- sensors ----------
		List<Sensor> sensorList = sensors == null ? null
				: sensors.stream().map(s -> toSensor(s, latest.get(s.getId()), now)).toList();

		// ---------- devices ----------
		List<DashboardResponse.Device> deviceList = devices == null ? null
				: devices.stream()
						.map(d -> toDevice(d, statusOf(d, sensors, latest, now)))
						.toList();

		// ---------- rooms ----------
		List<Room> rooms = (devices == null || sensors == null) ? null : buildRooms(devices, sensors, latest, now);

		// ---------- docker ----------
		List<DockerContainer> docker = dockerList == null ? null
				: dockerList.stream()
						.map(c -> new DockerContainer(c.getId(), c.getName(), c.getState(), c.getStatus()))
						.toList();

		// ---------- config + version ----------
		TabletConfig config = settings == null ? null : toConfig(settings);
		int cfgVersion = settings == null ? 0 : configVersion(settings);
		boolean sendConfig = config != null && (cfgVer == null || cfgVer != cfgVersion);

		return DashboardResponse.builder()
				.success(true)
				.ts(now)
				.cfgVersion(cfgVersion)
				.weather(buildWeather(weather, weather5, now, forecastHours(settings)))
				.rooms(rooms == null ? Section.failed() : Section.ok(rooms, now))
				.summary(buildSummary(rooms, docker, deviceList, sensorList))
				.docker(docker == null ? Section.failed() : Section.ok(docker, now))
				.devices(deviceList == null ? Section.failed() : Section.ok(deviceList, now))
				.sensors(sensorList == null ? Section.failed() : Section.ok(sensorList, now))
				.system(server == null ? Section.failed()
						: Section.ok(new SystemStats(round1(server.getCpu()), round1(server.getRam()), server.getUptime()), now))
				.red(red == null ? Section.<DashboardResponse.RedData>failed()
						: Section.ok(new DashboardResponse.RedData(red.total(),
								red.clients().stream()
										.map(c -> new DashboardResponse.RedClient(c.name(), c.ip(), c.mac(), c.type()))
										.toList()), now))
				.grafana(grafana == null ? Section.<DashboardResponse.GrafanaData>failed()
						: Section.ok(new DashboardResponse.GrafanaData(
								grafana.rootTotalGb(), grafana.rootUsedGb(), grafana.rootFreeGb(),
								grafana.storageTotalGb(), grafana.storageUsedGb(), grafana.storageFreeGb(),
								grafana.cpu(), grafana.ram(), grafana.tempC()), now))
				.config(sendConfig ? config : null)
				.version(buildProperties.getVersion())
				.build();
	}

	// =====================================================================
	// WEATHER
	// =====================================================================
	private Section<WeatherData> buildWeather(WeatherResponse w, Weather5diasResponse w5, long now, int[] hours) {
		if (w == null || w.getData() == null) {
			return Section.failed();
		}
		WeatherResponse.WeatherData d = w.getData();

		CurrentWeather current = new CurrentWeather(
				d.getLocation(), d.getTemperature(), d.getFeelsLike(),
				d.getMinTemperature(), d.getMaxTemperature(),
				d.getHumidity(), d.getPressure(), d.getDescription(), d.getIcon(),
				d.getWindSpeed(), d.getWindDirection(), d.getClouds(),
				d.getSunrise(), d.getSunset(),
				d.getTimezone() == null ? null : d.getTimezone().intValue());

		List<ForecastDay> days = null;
		if (w5 != null && w5.getForecast() != null) {
			days = tryGet("forecast", () -> WeatherForecastAggregator.toDaily(w5, 5, hours));
		}
		if (days == null) {
			days = List.of();
		}

		// weather.updatedAt viene en ms -> segundos
		long updated = d.getUpdatedAt() == null ? now : d.getUpdatedAt() / 1000;
		return Section.ok(new WeatherData(current, days), updated);
	}

	// =====================================================================
	// ROOMS (una por device.location, ultima medicion con temperatura)
	// =====================================================================
	private List<Room> buildRooms(List<DeviceDto> devices, List<SensorDto> sensors,
			Map<UUID, MeasurementDto> latest, long now) {

		Map<UUID, DeviceDto> deviceById = new HashMap<>();
		for (DeviceDto d : devices) {
			deviceById.put(d.getId(), d);
		}

		// location -> sensores de esa habitacion (respeta el orden de aparicion)
		Map<String, List<SensorDto>> byRoom = new LinkedHashMap<>();
		for (SensorDto s : sensors) {
			DeviceDto d = deviceById.get(s.getDeviceId());
			if (d == null || d.getLocation() == null || d.getLocation().isBlank()) {
				continue;
			}
			byRoom.computeIfAbsent(d.getLocation(), k -> new ArrayList<>()).add(s);
		}

		List<Room> rooms = new ArrayList<>();
		for (Map.Entry<String, List<SensorDto>> e : byRoom.entrySet()) {
			MeasurementDto best = null;
			Integer minInterval = null;

			for (SensorDto s : e.getValue()) {
				Integer interval = intervalOf(s);
				if (interval != null && (minInterval == null || interval < minInterval)) {
					minInterval = interval;
				}
				MeasurementDto m = latest.get(s.getId());
				if (m == null || m.getTemperature() == null || m.getCreatedAt() == null) {
					continue;
				}
				if (best == null || m.getCreatedAt().isAfter(best.getCreatedAt())) {
					best = m;
				}
			}

			Long last = best == null ? null : epoch(best.getCreatedAt());
			rooms.add(new Room(
					e.getKey(),
					best == null ? null : best.getTemperature(),
					best == null ? null : best.getHumidity(),
					last,
					isStale(last, minInterval, now)));
		}
		return rooms;
	}

	// =====================================================================
	// SUMMARY
	// =====================================================================
	private Summary buildSummary(List<Room> rooms, List<DockerContainer> docker,
			List<DashboardResponse.Device> devices, List<Sensor> sensors) {

		Double avgT = null;
		Double avgH = null;
		if (rooms != null) {
			avgT = average(rooms.stream().filter(r -> !r.stale()).map(Room::temperature).toList());
			avgH = average(rooms.stream().filter(r -> !r.stale()).map(Room::humidity).toList());
		}

		Counter dockerC = docker == null ? null
				: Counter.docker((int) docker.stream().filter(c -> "running".equals(c.state())).count(), docker.size());

		Counter devicesC = devices == null ? null
				: Counter.online((int) devices.stream().filter(d -> DeviceStatus.ONLINE.name().equals(d.status())).count(),
						devices.size());

		Counter sensorsC = sensors == null ? null
				: Counter.online((int) sensors.stream().filter(s -> s.enabled() && !s.stale()).count(), sensors.size());

		return new Summary(avgT, avgH, dockerC, devicesC, sensorsC);
	}

	// =====================================================================
	// MAPPERS
	// =====================================================================
	private DashboardResponse.Device toDevice(DeviceDto d, String status) {
		DeviceConfigDto cfg = d.getDeviceConfig();
		return new DashboardResponse.Device(
				str(d.getId()),
				d.getName(),
				d.getDescription(),
				d.getHardwareModel(),
				d.getLocation(),
				status,
				d.getDeviceType() == null ? null : d.getDeviceType().name(),
				cfg == null ? null : cfg.getIpAddress(),
				cfg == null ? null : cfg.getMacAddress(),
				cfg == null ? null : cfg.getFirmwareVersion()
		// secretKey NO se expone
		);
	}

	private Sensor toSensor(SensorDto s, MeasurementDto m, long now) {
		SensorConfigDto cfg = s.getSensorConfig();
		Integer interval = intervalOf(s);
		boolean enabled = s.isEnabled() && (cfg == null || cfg.getEnabled() == null || cfg.getEnabled());
		Long last = (m == null || m.getCreatedAt() == null) ? null : epoch(m.getCreatedAt());

		return new Sensor(
				str(s.getId()),
				str(s.getDeviceId()),
				s.getName(),
				s.getType() == null ? null : s.getType().name(),
				enabled,
				interval,
				m == null ? null : m.getTemperature(),
				m == null ? null : m.getHumidity(),
				last,
				isStale(last, interval, now));
	}

	private TabletConfig toConfig(SettingsData s) {
		return new TabletConfig(
				s.apiRefreshSeconds(), s.weatherRefreshSeconds(), s.systemRefreshSeconds(),
				s.mqttReconnectSeconds(), s.deviceName(), s.deviceLocation(),
				s.brightness(), s.screenAlwaysOn(), s.screenTimeoutSeconds(),
				s.forecastHour1(), s.forecastHour2(), s.forecastHour3());
	}

	/** Version estable derivada de los valores: cambia solo si cambia algun ajuste. */
	private int configVersion(SettingsData s) {
		return Objects.hash(
				s.apiRefreshSeconds(), s.weatherRefreshSeconds(), s.systemRefreshSeconds(),
				s.mqttReconnectSeconds(), s.deviceName(), s.deviceLocation(),
				s.brightness(), s.screenAlwaysOn(), s.screenTimeoutSeconds(),
				s.forecastHour1(), s.forecastHour2(), s.forecastHour3());
	}

	/** Horas de referencia de las franjas del pronostico (ordenadas, sin repetir). */
	private int[] forecastHours(SettingsData s) {
		if (s == null) {
			return WeatherForecastAggregator.DEFAULT_HOURS;
		}
		return java.util.stream.IntStream.of(s.forecastHour1(), s.forecastHour2(), s.forecastHour3())
				.distinct().sorted().toArray();
	}

	// =====================================================================
	// HELPERS
	// =====================================================================

	/**
	 * Ultima medicion por sensor.
	 * TODO: measurementService.findAll() devuelve TODO el historico y crecera sin
	 * limite. Sustituir por una query "ultima medicion por sensor" en el repositorio.
	 */
	private Map<UUID, MeasurementDto> latestBySensor(List<MeasurementDto> measurements) {
		Map<UUID, MeasurementDto> map = new HashMap<>();
		if (measurements == null) {
			return map;
		}
		for (MeasurementDto m : measurements) {
			if (m.getSensor() == null || m.getSensor().getId() == null || m.getCreatedAt() == null) {
				continue;
			}
			map.merge(m.getSensor().getId(), m, (a, b) -> b.getCreatedAt().isAfter(a.getCreatedAt()) ? b : a);
		}
		return map;
	}

	private Integer intervalOf(SensorDto s) {
		return s.getSensorConfig() == null ? null : s.getSensorConfig().getInterval();
	}

	// ---- ONLINE (inicio) ----

	/** Margen de silencio permitido: 2 intervalos, nunca menos de 15 min (deep sleep + WiFi). */
	private long toleranceSeconds(Integer intervalSeconds) {
		int interval = intervalSeconds == null ? DEFAULT_INTERVAL_SECONDS : intervalSeconds;
		return Math.max(2L * interval, MIN_TOLERANCE_SECONDS);
	}

	/** Sin dato, o mas viejo que el margen => stale / sin señal. */
	private boolean isStale(Long lastEpoch, Integer intervalSeconds, long now) {
		if (lastEpoch == null) {
			return true;
		}
		return now - lastEpoch > toleranceSeconds(intervalSeconds);
	}

	/**
	 * Estado REAL del device. El campo status de la BD nadie lo pasa a OFFLINE,
	 * asi que se calcula: online solo si hubo señal de vida reciente
	 * (lastSeen o ultima medicion de alguno de sus sensores).
	 */
	private String statusOf(DeviceDto d, List<SensorDto> sensors,
			Map<UUID, MeasurementDto> latest, long now) {

		if (d.getStatus() == DeviceStatus.ERROR) {
			return DeviceStatus.ERROR.name();
		}
		if (d.getEnabled() != null && !d.getEnabled()) {
			return DeviceStatus.OFFLINE.name();
		}

		Long last = d.getLastSeen() == null ? null : epoch(d.getLastSeen());
		Integer expected = null; // silencio esperado: el mayor entre intervalos y heartbeat

		DeviceConfigDto cfg = d.getDeviceConfig();
		if (cfg != null && cfg.getHeartbeat() != null) {
			expected = cfg.getHeartbeat().heartbeatSeconds();
		}

		if (sensors != null) {
			for (SensorDto s : sensors) {
				if (!Objects.equals(s.getDeviceId(), d.getId())) {
					continue;
				}
				Integer interval = intervalOf(s);
				if (interval != null && (expected == null || interval > expected)) {
					expected = interval;
				}
				MeasurementDto m = latest.get(s.getId());
				if (m != null && m.getCreatedAt() != null) {
					long t = epoch(m.getCreatedAt());
					if (last == null || t > last) {
						last = t;
					}
				}
			}
		}

		return isStale(last, expected, now) ? DeviceStatus.OFFLINE.name() : DeviceStatus.ONLINE.name();
	}

	// ---- ONLINE (fin) ----

	private long epoch(LocalDateTime t) {
		return t.atZone(ZoneId.systemDefault()).toEpochSecond();
	}

	private String str(UUID id) {
		return id == null ? null : id.toString();
	}

	private Double average(List<Double> values) {
		List<Double> v = values.stream().filter(Objects::nonNull).toList();
		if (v.isEmpty()) {
			return null;
		}
		return round1(v.stream().mapToDouble(Double::doubleValue).average().orElse(0));
	}

	private double round1(double v) {
		return Math.round(v * 10.0) / 10.0;
	}

	/** Ejecuta una fuente; si falla, loguea y devuelve null en vez de romper el dashboard. */
	private <T> T tryGet(String name, Callable<T> call) {
		try {
			return call.call();
		} catch (Exception e) {
			log.warn("Dashboard: fallo al obtener '{}': {}", name, e.getMessage());
			return null;
		}
	}
}
