package com.bahguta.bahguta_api.system.settings.service;

import java.time.Instant;

import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.bahguta.bahguta_api.system.settings.dto.SettingsData;
import com.bahguta.bahguta_api.system.settings.dto.SettingsUpdateRequest;
import com.bahguta.bahguta_api.system.settings.entity.AppSettingsEntity;
import com.bahguta.bahguta_api.system.settings.repository.AppSettingsRepository;

import lombok.RequiredArgsConstructor;

/**
 * Acceso a la configuracion global guardada en la tabla app_settings.
 * Lo usa SettingsService (lectura) y el controller (actualizacion).
 */
@Service
@RequiredArgsConstructor
public class SettingsStore {

    private final AppSettingsRepository repository;

    @Transactional
    public SettingsData get() {
        return toData(loadOrCreate());
    }

    /**
     * Aplica un cambio (parcial o completo) y devuelve los ajustes resultantes.
     * IllegalArgumentException si algun valor esta fuera de rango.
     */
    @Transactional
    public SettingsData update(SettingsUpdateRequest request) {
        AppSettingsEntity entity = loadOrCreate();

        SettingsData merged = SettingsLogic.merge(toData(entity), request);

        entity.setSensorIntervalSeconds(merged.apiRefreshSeconds());
        entity.setWeatherRefreshSeconds(merged.weatherRefreshSeconds());
        entity.setSystemRefreshSeconds(merged.systemRefreshSeconds());
        entity.setMqttReconnectSeconds(merged.mqttReconnectSeconds());
        entity.setDeviceName(merged.deviceName());
        entity.setDeviceLocation(merged.deviceLocation());
        entity.setBrightness(merged.brightness());
        entity.setScreenAlwaysOn(merged.screenAlwaysOn());
        entity.setScreenTimeoutSeconds(merged.screenTimeoutSeconds());
        entity.setForecastHour1(merged.forecastHour1());
        entity.setForecastHour2(merged.forecastHour2());
        entity.setForecastHour3(merged.forecastHour3());
        entity.setUpdatedAt(Instant.now());

        return toData(repository.save(entity));
    }

    private AppSettingsEntity loadOrCreate() {
        return repository.findById(AppSettingsEntity.SINGLETON_ID)
                .orElseGet(() -> repository.save(defaults()));
    }

    /** Valores iniciales = los que tenia el sistema antes de existir la tabla. */
    private AppSettingsEntity defaults() {
        AppSettingsEntity e = new AppSettingsEntity();
        e.setSensorIntervalSeconds(30);
        e.setWeatherRefreshSeconds(900);
        e.setSystemRefreshSeconds(30);
        e.setMqttReconnectSeconds(10);
        e.setDeviceName("ESP32 C3 Super mini.");
        e.setDeviceLocation("Despacho");
        e.setBrightness(200);
        e.setScreenAlwaysOn(true);
        e.setScreenTimeoutSeconds(0);
        e.setForecastHour1(9);
        e.setForecastHour2(14);
        e.setForecastHour3(19);
        return e;
    }

    private SettingsData toData(AppSettingsEntity e) {
        return new SettingsData(
                e.getSensorIntervalSeconds(),
                e.getWeatherRefreshSeconds(),
                e.getSystemRefreshSeconds(),
                e.getMqttReconnectSeconds(),
                e.getDeviceName(),
                e.getDeviceLocation(),
                e.getBrightness(),
                e.isScreenAlwaysOn(),
                e.getScreenTimeoutSeconds(),
                e.getForecastHour1(),
                e.getForecastHour2(),
                e.getForecastHour3());
    }
}
