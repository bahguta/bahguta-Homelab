package com.bahguta.bahguta_api.system.server.service;

import java.io.IOException;
import java.net.URI;
import java.net.URLEncoder;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.config.BahgutaProperties;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Service
@RequiredArgsConstructor
public class PrometheusClientImpl implements PrometheusClient {

    private final BahgutaProperties properties;
    private final ObjectMapper objectMapper;

    private final HttpClient httpClient = HttpClient.newHttpClient();

    @Override
    public double query(String query) {
        JsonNode result = fetchResult(query);

        if (!result.isArray() || result.isEmpty()) {
            throw new RuntimeException("Prometheus returned no data.");
        }

        JsonNode value = result.get(0).path("value");

        if (!value.isArray() || value.size() < 2) {
            throw new RuntimeException("Invalid Prometheus response.");
        }

        return value.get(1).asDouble();
    }

    @Override
    public List<PromSample> queryVector(String query) {
        JsonNode result = fetchResult(query);

        List<PromSample> samples = new ArrayList<>();

        if (!result.isArray()) {
            return samples;
        }

        for (JsonNode item : result) {
            JsonNode value = item.path("value");

            if (!value.isArray() || value.size() < 2) {
                continue;
            }

            Map<String, String> labels = new HashMap<>();
            Iterator<Map.Entry<String, JsonNode>> fields = item.path("metric").fields();

            while (fields.hasNext()) {
                Map.Entry<String, JsonNode> f = fields.next();
                labels.put(f.getKey(), f.getValue().asText());
            }

            samples.add(new PromSample(labels, value.get(1).asDouble()));
        }

        return samples;
    }

    /** Lanza la query y devuelve data.result (RuntimeException si algo falla). */
    private JsonNode fetchResult(String query) {
        try {
            String url = properties.getPrometheus().getUrl()
                    + "/api/v1/query?query="
                    + URLEncoder.encode(query, StandardCharsets.UTF_8);

            log.debug("Prometheus query: {}", query);

            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(url))
                    .GET()
                    .build();

            HttpResponse<String> response =
                    httpClient.send(request, HttpResponse.BodyHandlers.ofString());

            if (response.statusCode() != 200) {
                log.error("Prometheus returned HTTP {}", response.statusCode());
                throw new RuntimeException("Prometheus request failed.");
            }

            JsonNode root = objectMapper.readTree(response.body());

            if (!"success".equals(root.path("status").asText())) {
                throw new RuntimeException("Prometheus query failed.");
            }

            return root.path("data").path("result");

        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            throw new RuntimeException("Prometheus query interrupted.", e);

        } catch (IOException e) {
            log.error("Error querying Prometheus", e);
            throw new RuntimeException("Unable to query Prometheus.", e);
        }
    }
}
