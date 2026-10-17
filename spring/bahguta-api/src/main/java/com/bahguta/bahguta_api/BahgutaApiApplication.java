package com.bahguta.bahguta_api;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.context.properties.EnableConfigurationProperties;

import com.bahguta.bahguta_api.config.BahgutaProperties;

@SpringBootApplication
@EnableConfigurationProperties(BahgutaProperties.class)
public class BahgutaApiApplication {

	public static void main(String[] args) {
		SpringApplication.run(BahgutaApiApplication.class, args);
	}

}
