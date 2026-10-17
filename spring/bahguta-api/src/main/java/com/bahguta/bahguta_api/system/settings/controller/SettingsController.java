package com.bahguta.bahguta_api.system.settings.controller;

import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;
import com.bahguta.bahguta_api.common.api.ApiResponse;
import com.bahguta.bahguta_api.common.api.ApiResponses;
import com.bahguta.bahguta_api.system.settings.dto.SettingsData;
import com.bahguta.bahguta_api.system.settings.dto.SettingsUpdateRequest;
import com.bahguta.bahguta_api.system.settings.service.SettingsStore;
import lombok.RequiredArgsConstructor;


@RestController
@RequestMapping("/api/v1/settings")
@RequiredArgsConstructor
public class SettingsController {


    private final SettingsStore settingsStore;

    @RequestMapping(method = { RequestMethod.POST, RequestMethod.PUT, RequestMethod.PATCH })
    public ApiResponse<SettingsData> updateSettings(@RequestBody SettingsUpdateRequest request) {
        return ApiResponses.ok(settingsStore.update(request));
    }

}

