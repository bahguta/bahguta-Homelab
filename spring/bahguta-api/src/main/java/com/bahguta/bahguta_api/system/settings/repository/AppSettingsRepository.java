package com.bahguta.bahguta_api.system.settings.repository;

import org.springframework.data.jpa.repository.JpaRepository;

import com.bahguta.bahguta_api.system.settings.entity.AppSettingsEntity;

public interface AppSettingsRepository extends JpaRepository<AppSettingsEntity, Integer> {
}
