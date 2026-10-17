package com.bahguta.bahguta_api.weather.dto;


import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class WeatherRequest {

    private String city;
    private String country;
}