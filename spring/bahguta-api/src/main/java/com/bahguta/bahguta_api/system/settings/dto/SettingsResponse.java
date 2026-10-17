package com.bahguta.bahguta_api.system.settings.dto;

import lombok.Builder;

@Builder
public record SettingsResponse(boolean success, SettingsData data) {}


