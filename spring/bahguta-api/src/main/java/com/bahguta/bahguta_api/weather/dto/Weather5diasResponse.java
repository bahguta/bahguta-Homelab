package com.bahguta.bahguta_api.weather.dto;

import java.util.List;


import lombok.Builder;
import lombok.Data;
import lombok.Value;

@Data
@Builder
public class Weather5diasResponse {

	 boolean success;
	    String location;
	    String country;
	    Long timezone;
	    Long sunrise;
	    Long sunset;
	    List<ForecastData> forecast;

	    @Value
	    @Builder
	    @Data
	    public static class ForecastData {
	        Long dt;
	        String dtTxt;
	        Double temperature;
	        Double feelsLike;
	        Double minTemperature;
	        Double maxTemperature;
	        Integer humidity;
	        Integer pressure;
	        String description;
	        String icon;
	        Double windSpeed;
	        Integer windDirection;
	        Integer clouds;
	        Double rain3h; // Puede ser null si no hay lluvia en ese tramo
	        Long updatedAt;
	    }
}
