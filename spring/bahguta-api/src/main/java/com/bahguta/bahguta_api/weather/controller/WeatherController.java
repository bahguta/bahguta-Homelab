package com.bahguta.bahguta_api.weather.controller;

import com.bahguta.bahguta_api.weather.dto.Weather5diasResponse;
import com.bahguta.bahguta_api.weather.dto.WeatherResponse;
import com.bahguta.bahguta_api.weather.service.WeatherService;
import lombok.RequiredArgsConstructor;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api/v1/weather")
@RequiredArgsConstructor
public class WeatherController {

    private final WeatherService weatherService;

    @GetMapping
    public WeatherResponse getWeather() {

        return weatherService.getWeather();
    }
    
    
    @GetMapping("/5dias")
    public Weather5diasResponse getWeather5days() {

        return weatherService.getWeather5days();
    }
}