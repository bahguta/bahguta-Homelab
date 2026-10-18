package com.bahguta.bahguta_api.common.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import io.swagger.v3.oas.models.ExternalDocumentation;
import io.swagger.v3.oas.models.OpenAPI;
import io.swagger.v3.oas.models.info.Contact;
import io.swagger.v3.oas.models.info.Info;

@Configuration
public class OpenApiConfig {

    @Bean
    OpenAPI bahgutaOpenAPI() {

        return new OpenAPI()

                .info(new Info()
                        .title("bahguta Homelab API")
                        .description("Backend REST API for the bahguta Homelab platform")
                        .version("1.0.0")
                        .contact(new Contact()
                                .name("bahguta Homelab")))

                .externalDocs(new ExternalDocumentation()
                        .description("bahguta Documentation"));

    }

}