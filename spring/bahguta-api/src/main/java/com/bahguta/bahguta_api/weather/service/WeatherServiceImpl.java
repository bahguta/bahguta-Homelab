package com.bahguta.bahguta_api.weather.service;

import com.bahguta.bahguta_api.weather.dto.OpenWeather5diasResponse;
import com.bahguta.bahguta_api.weather.dto.OpenWeatherResponse;
import com.bahguta.bahguta_api.weather.dto.Weather5diasResponse;
import com.bahguta.bahguta_api.weather.dto.WeatherResponse;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestClient;

@Service
public class WeatherServiceImpl implements WeatherService {

	private static final long CACHE_TIME = 30 * 60 * 1000;

	private final RestClient restClient;

	@Value("${bahguta.weather.api-key}")
	private String apiKey;

	@Value("${bahguta.weather.city}")
	private String city;

	@Value("${bahguta.weather.country}")
	private String country;

	private WeatherResponse cachedWeather;
	private long lastUpdate = 0;
	
	private Weather5diasResponse cachedForecast;
	private long lastForecastUpdate;

	public WeatherServiceImpl(RestClient.Builder restClientBuilder) {
        this.restClient = restClientBuilder.build();
    }

	public synchronized WeatherResponse getWeather() {

		long now = System.currentTimeMillis();

		if (cachedWeather != null && now - lastUpdate < CACHE_TIME) {

			return cachedWeather;
		}

		OpenWeatherResponse response = restClient.get()
				.uri(uriBuilder -> uriBuilder.scheme("https").host("api.openweathermap.org").path("/data/2.5/weather")
						.queryParam("q", city + "," + country).queryParam("appid", apiKey).queryParam("units", "metric")
						.queryParam("lang", "es").build())
				.retrieve().body(OpenWeatherResponse.class);

		if (response == null) {
			throw new IllegalStateException("OpenWeather returned an empty response");
		}

		cachedWeather = map(response);
		lastUpdate = now;

		return cachedWeather;
	}

	private WeatherResponse map(OpenWeatherResponse source) {

		OpenWeatherResponse.Weather weather = source.getWeather() != null && !source.getWeather().isEmpty()
				? source.getWeather().get(0)
				: null;

		WeatherResponse.WeatherData data = WeatherResponse.WeatherData.builder()
				.location(source.getName())
				.country(source.getSys() != null ? source.getSys().getCountry() : null)
				.temperature(source.getMain() != null ? source.getMain().getTemp() : null)
				.feelsLike(source.getMain() != null ? source.getMain().getFeels_like() : null)
				.minTemperature(source.getMain() != null ? source.getMain().getTemp_min() : null)
				.maxTemperature(source.getMain() != null ? source.getMain().getTemp_max() : null)
				.humidity(source.getMain() != null ? source.getMain().getHumidity() : null)
				.pressure(source.getMain() != null ? source.getMain().getPressure() : null)
				.description(weather != null ? weather.getDescription() : null)
				.icon(weather != null ? weather.getIcon() : null)
				.windSpeed(source.getWind() != null ? source.getWind().getSpeed() : null)
				.windDirection(source.getWind() != null ? source.getWind().getDeg() : null)
				.clouds(source.getClouds() != null ? source.getClouds().getAll() : null)
				.sunrise(source.getSys() != null ? source.getSys().getSunrise() : null)
				.sunset(source.getSys() != null ? source.getSys().getSunset() : null)
				.timezone(source.getTimezone().longValue())
				.updatedAt(System.currentTimeMillis())
				.build();

		return new WeatherResponse(true, data);
	}
	
	
	@Override
	public synchronized Weather5diasResponse getWeather5days () {
	    long now = System.currentTimeMillis();

	    // Reutiliza la misma lógica de caché que tu método actual
	    if (cachedForecast != null && now - lastForecastUpdate < CACHE_TIME) {
	        return cachedForecast;
	    }

	    // Petición al endpoint /2.5/forecast con los mismos parámetros
	    OpenWeather5diasResponse response = restClient.get()
	            .uri(uriBuilder -> uriBuilder.scheme("https").host("api.openweathermap.org").path("/data/2.5/forecast")
	                    .queryParam("q", city + "," + country).queryParam("appid", apiKey).queryParam("units", "metric")
	                    .queryParam("lang", "es").build())
	            .retrieve().body(OpenWeather5diasResponse.class);

	    if (response == null) {
	        throw new IllegalStateException("OpenWeather forecast returned an empty response");
	    }

	    // Mapeo e invalidación de caché
	    cachedForecast = map(response);
	    lastForecastUpdate = now;

	    return cachedForecast;
	}

	private Weather5diasResponse map(OpenWeather5diasResponse source) {
	    long now = System.currentTimeMillis();
	    
	    // Extraemos la información general de la ciudad (evita npe si la respuesta falla)
	    String location = source.getCity() != null ? source.getCity().getName() : null;
	    String country = source.getCity() != null ? source.getCity().getCountry() : null;
	    Long timezone = source.getCity() != null ? (long) source.getCity().getTimezone() : null;
	    Long sunrise = source.getCity() != null ? source.getCity().getSunrise() : null;
	    Long sunset = source.getCity() != null ? source.getCity().getSunset() : null;

	    // Transformamos cada elemento de la lista de 3 horas de OpenWeather a nuestro formato limpio
	    java.util.List<Weather5diasResponse.ForecastData> forecastList = java.util.Collections.emptyList();
	    
	    if (source.getList() != null) {
	        forecastList = source.getList().stream()
	            .map(item -> {
	                // Extraer el primer elemento del array weather si existe
	                OpenWeather5diasResponse.Weather weather = item.getWeather() != null && !item.getWeather().isEmpty()
	                        ? item.getWeather().get(0)
	                        : null;

	                return Weather5diasResponse.ForecastData.builder()
	                        .dt(item.getDt())
	                        .dtTxt(item.getDt_txt())
	                        .temperature(item.getMain() != null ? item.getMain().getTemp() : null)
	                        .feelsLike(item.getMain() != null ? item.getMain().getFeels_like() : null)
	                        .minTemperature(item.getMain() != null ? item.getMain().getTemp_min() : null)
	                        .maxTemperature(item.getMain() != null ? item.getMain().getTemp_max() : null)
	                        .humidity(item.getMain() != null ? item.getMain().getHumidity() : null)
	                        .pressure(item.getMain() != null ? item.getMain().getPressure() : null)
	                        .description(weather != null ? weather.getDescription() : null)
	                        .icon(weather != null ? weather.getIcon() : null)
	                        .windSpeed(item.getWind() != null ? item.getWind().getSpeed() : null)
	                        .windDirection(item.getWind() != null ? item.getWind().getDeg() : null)
	                        .clouds(item.getClouds() != null ? item.getClouds().getAll() : null)
	                        .rain3h(item.getRain() != null ? item.getRain().getThreeHours() : null)
	                        .updatedAt(now)
	                        .build();
	            })
	            .collect(java.util.stream.Collectors.toList());
	    }

	    return Weather5diasResponse.builder()
	            .success(true)
	            .location(location)
	            .country(country)
	            .timezone(timezone)
	            .sunrise(sunrise)
	            .sunset(sunset)
	            .forecast(forecastList)
	            .build();
	}


}