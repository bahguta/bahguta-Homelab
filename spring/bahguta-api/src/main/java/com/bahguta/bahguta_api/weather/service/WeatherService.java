package com.bahguta.bahguta_api.weather.service;

import com.bahguta.bahguta_api.weather.dto.Weather5diasResponse;
import com.bahguta.bahguta_api.weather.dto.WeatherResponse;

public interface WeatherService {
	
	WeatherResponse getWeather();

	Weather5diasResponse getWeather5days();
}
