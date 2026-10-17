package com.bahguta.bahguta_api.weather.dto;

import java.util.List;

import com.fasterxml.jackson.annotation.JsonProperty;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class OpenWeather5diasResponse {
	
	private String cod;
    private int message;
    private int cnt;
    private List<ForecastItem> list;
    private City city;

    @Getter
    @Setter
    public static class ForecastItem {
        private long dt;
        private MainData main;
        private List<Weather> weather;
        private Clouds clouds;
        private Wind wind;
        private int visibility;
        private double pop;
        private Rain rain; // Puede ser nulo si no llueve
        private Sys sys;
        private String dt_txt;
    }

    @Getter
    @Setter
    public static class MainData {
        private double temp;
        private double feels_like;
        private double temp_min;
        private double temp_max;
        private int pressure;
        private int sea_level;
        private int grnd_level;
        private int humidity;
        private double temp_kf;
    }

    @Getter
    @Setter
    public static class Weather {
        private int id;
        private String main;
        private String description;
        private String icon;
    }

    @Getter
    @Setter
    public static class Clouds {
        private int all;
    }

    @Getter
    @Setter
    public static class Wind {
        private double speed;
        private int deg;
        private double gust;
    }

    @Getter
    @Setter
    public static class Rain {
        @JsonProperty("3h")
        private double threeHours;
        public double getThreeHours() { return threeHours; }
        public void setThreeHours(double threeHours) { this.threeHours = threeHours; }
    }

    @Getter
    @Setter
    public static class Sys {
        private String pod;
    }

    @Getter
    @Setter
    public static class City {
        private int id;
        private String name;
        private Coord coord;
        private String country;
        private int population;
        private int timezone;
        private long sunrise;
        private long sunset;
    }

    @Getter
    @Setter
    public static class Coord {
        private double lat;
        private double lon;
    }

}
