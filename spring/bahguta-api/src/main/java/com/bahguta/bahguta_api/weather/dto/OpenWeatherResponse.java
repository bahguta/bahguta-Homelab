package com.bahguta.bahguta_api.weather.dto;

import lombok.Getter;
import lombok.Setter;

import java.util.List;

@Getter
@Setter
public class OpenWeatherResponse {

    private Coord coord;
    private List<Weather> weather;
    private Main main;
    private Wind wind;
    private Clouds clouds;
    private Sys sys;
    private String name;
    private Integer timezone;

    @Getter
    @Setter
    public static class Coord {
        private Double lon;
        private Double lat;
    }

    @Getter
    @Setter
    public static class Weather {
        private Integer id;
        private String main;
        private String description;
        private String icon;
    }

    @Getter
    @Setter
    public static class Main {
        private Double temp;
        private Double feels_like;
        private Double temp_min;
        private Double temp_max;
        private Integer pressure;
        private Integer humidity;
    }

    @Getter
    @Setter
    public static class Wind {
        private Double speed;
        private Integer deg;
        private Double gust;
    }

    @Getter
    @Setter
    public static class Clouds {
        private Integer all;
    }

    @Getter
    @Setter
    public static class Sys {
        private String country;
        private Long sunrise;
        private Long sunset;
    }
}