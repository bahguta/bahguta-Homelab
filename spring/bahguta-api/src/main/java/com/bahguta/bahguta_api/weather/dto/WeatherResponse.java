package com.bahguta.bahguta_api.weather.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;

@Builder
@Getter
@AllArgsConstructor
public class WeatherResponse {

    private boolean success;
    private WeatherData data;

    @Builder
    @Getter
    @AllArgsConstructor
    @NoArgsConstructor
    public static class WeatherData {

        private String location;
        private String country;

        private Double temperature;
        private Double feelsLike;
        private Double minTemperature;
        private Double maxTemperature;

        private Integer humidity;
        private Integer pressure;

        private String description;
        private String icon;

        private Double windSpeed;
        private Integer windDirection;

        private Integer clouds;

        private Long sunrise;
        private Long sunset;

        private Long timezone;

        private Long updatedAt;
    }
}