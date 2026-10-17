package com.bahguta.bahguta_api.system.settings.service;

import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.system.settings.dto.SettingsData;

import lombok.RequiredArgsConstructor;

@Service
@RequiredArgsConstructor
public class SettingsServiceImpl implements SettingsService {
	
	private final SettingsStore store;

	@Override
	public SettingsData getSettings() {
		return store.get();
	}

}
