package com.bahguta.bahguta_api.system.network.service;

import java.io.IOException;
import java.net.CookieManager;
import java.net.URI;
import java.net.Socket;
import java.net.URLEncoder;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.charset.StandardCharsets;
import java.security.SecureRandom;
import java.security.cert.X509Certificate;
import java.time.Duration;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import javax.net.ssl.SSLEngine;
import javax.net.ssl.X509ExtendedTrustManager;

import org.springframework.stereotype.Service;

import com.bahguta.bahguta_api.config.BahgutaProperties;
import com.bahguta.bahguta_api.system.network.dto.NetworkClientsDto;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

/**
 * Lista de equipos conectados, leida de la API web de Omada Controller
 * (login con usuario/contrasena, igual que la interfaz web).
 *
 * - Usuario, contrasena y URL salen de bahguta.omada.* (variables de entorno).
 * - Cache de 15 s: la tablet pregunta cada ~30 s y Omada no necesita mas.
 * - Si falla (login, red...) devuelve null y no reintenta durante 60 s, para
 *   no bloquear la cuenta ni retrasar el dashboard.
 */
@Slf4j
@Service
@RequiredArgsConstructor
public class OmadaClientService {

    private static final long CACHE_MS = 15_000;
    private static final long RETRY_AFTER_FAIL_MS = 60_000;
    private static final Duration TIMEOUT = Duration.ofSeconds(4);

    private final BahgutaProperties properties;
    private final ObjectMapper mapper;

    private HttpClient http;
    private String prefix;   // "/{omadacId}"
    private String token;
    private String siteId;

    private NetworkClientsDto cached;
    private long cachedAt;
    private long blockedUntil;
    private boolean notConfiguredLogged;

    /** null si no esta configurado, hubo un fallo reciente o no se pudo leer. */
    public synchronized NetworkClientsDto getClients() {
        BahgutaProperties.Omada cfg = properties.getOmada();

        if (cfg == null || blank(cfg.getUrl()) || blank(cfg.getUsername()) || blank(cfg.getPassword())) {
            if (!notConfiguredLogged) {
                notConfiguredLogged = true;
                log.warn("Omada no configurado: faltan variables {}{}{} en el contenedor (la seccion red queda vacia).",
                        cfg == null || blank(cfg.getUrl()) ? "OMADA_URL " : "",
                        cfg == null || blank(cfg.getUsername()) ? "OMADA_USERNAME " : "",
                        cfg == null || blank(cfg.getPassword()) ? "OMADA_API_PASSWORD" : "");
            }
            return null;
        }

        long now = System.currentTimeMillis();

        if (cached != null && now - cachedAt < CACHE_MS) {
            return cached;
        }
        if (now < blockedUntil) {
            return null;
        }

        try {
            NetworkClientsDto result;

            try {
                result = fetch(cfg);
            } catch (SessionException e) {
                resetSession();
                result = fetch(cfg);
            }

            cached = result;
            cachedAt = now;
                        
            return result;

        } catch (Exception e) {
            if (e instanceof InterruptedException) {
                Thread.currentThread().interrupt();
            }
            log.warn("Omada: no se pudo leer la lista de clientes ({}). Reintento en {} s.",
                    e.getMessage(), RETRY_AFTER_FAIL_MS / 1000);
            resetSession();
            blockedUntil = now + RETRY_AFTER_FAIL_MS;
            return null;
        }
    }

    // =====================================================================

    private NetworkClientsDto fetch(BahgutaProperties.Omada cfg) throws Exception {
        String base = cfg.getUrl().replaceAll("/+$", "");

        if (token == null) {
            login(base, cfg);
        }
        if (siteId == null) {
            siteId = findSite(base, cfg.getSite());
        }

        JsonNode root = get(base, prefix + "/api/v2/sites/" + siteId
                + "/clients?currentPage=1&currentPageSize=200&filters.active=true");

        List<NetworkClientsDto.Client> clients = new ArrayList<>();

        for (JsonNode c : root.path("result").path("data")) {
            String mac = text(c, "mac");
            String name = text(c, "name");
            if (name.isBlank()) {
                name = text(c, "hostName");
            }
            if (name.isBlank()) {
                name = mac;
            }

            String type = "";
            if (c.has("wireless")) {
                type = c.get("wireless").asBoolean() ? "wifi" : "cable";
            }

            clients.add(new NetworkClientsDto.Client(name, text(c, "ip"), mac, type));
        }

        clients.sort(Comparator.comparing(NetworkClientsDto.Client::name, String.CASE_INSENSITIVE_ORDER));

        return new NetworkClientsDto(clients.size(), clients);
    }

    private void login(String base, BahgutaProperties.Omada cfg) throws Exception {
        // 1) omadacId (en versiones muy antiguas no existe: prefijo vacio)
        JsonNode info = send(HttpRequest.newBuilder()
                .uri(URI.create(base + "/api/info"))
                .timeout(TIMEOUT).GET().build());
        String omadacId = text(info.path("result"), "omadacId");
        prefix = omadacId.isBlank() ? "" : "/" + omadacId;

        // 2) login: devuelve el token CSRF y deja la cookie de sesion en el CookieManager
        ObjectNode body = mapper.createObjectNode();
        body.put("username", cfg.getUsername());
        body.put("password", cfg.getPassword());

        JsonNode res = send(HttpRequest.newBuilder()
                .uri(URI.create(base + prefix + "/api/v2/login"))
                .timeout(TIMEOUT)
                .header("Content-Type", "application/json")
                .POST(HttpRequest.BodyPublishers.ofString(body.toString()))
                .build());

        if (res.path("errorCode").asInt(-1) != 0) {
            throw new IllegalStateException("login rechazado: " + text(res, "msg"));
        }

        token = text(res.path("result"), "token");
        if (token.isBlank()) {
            throw new IllegalStateException("login sin token");
        }
    }

    private String findSite(String base, String wanted) throws Exception {
        JsonNode root = get(base, prefix + "/api/v2/sites?currentPage=1&currentPageSize=100");

        String first = null;
        for (JsonNode s : root.path("result").path("data")) {
            String id = text(s, "id");
            if (first == null) {
                first = id;
            }
            if (!blank(wanted) && wanted.equalsIgnoreCase(text(s, "name"))) {
                return id;
            }
        }
        if (blank(wanted) && first != null) {
            return first;
        }

        // Cuentas con permisos limitados: los sitios vienen en users/current
        JsonNode current = get(base, prefix + "/api/v2/users/current");
        for (JsonNode s : current.path("result").path("privilege").path("sites")) {
            String key = text(s, "key");
            if (blank(wanted) || wanted.equalsIgnoreCase(text(s, "name"))) {
                return key;
            }
        }
        

        throw new IllegalStateException("sitio de Omada no encontrado: " + (blank(wanted) ? "(primero)" : wanted));
    }

    /** GET con token (cabecera + parametro); errorCode != 0 => sesion invalida. */
    private JsonNode get(String base, String path) throws Exception {
        String sep = path.contains("?") ? "&" : "?";

        JsonNode res = send(HttpRequest.newBuilder()
                .uri(URI.create(base + path + sep + "token=" + URLEncoder.encode(token, StandardCharsets.UTF_8)))
                .timeout(TIMEOUT)
                .header("Csrf-Token", token)
                .GET().build());

        if (res.path("errorCode").asInt(-1) != 0) {
            throw new SessionException("Omada errorCode=" + res.path("errorCode").asInt(-1) + " " + text(res, "msg"));
        }
        return res;
    }

    private JsonNode send(HttpRequest request) throws IOException, InterruptedException {
        HttpResponse<String> response = client().send(request, HttpResponse.BodyHandlers.ofString());

        if (response.statusCode() == 401 || response.statusCode() == 403) {
            throw new SessionException("HTTP " + response.statusCode());
        }
        if (response.statusCode() != 200) {
            throw new IOException("HTTP " + response.statusCode() + " en " + request.uri().getPath());
        }
        return mapper.readTree(response.body());
    }

    private HttpClient client() {
        if (http == null) {
            HttpClient.Builder builder = HttpClient.newBuilder()
                    .cookieHandler(new CookieManager())
                    .connectTimeout(TIMEOUT);

            BahgutaProperties.Omada cfg = properties.getOmada();

            if (cfg != null && cfg.isInsecureTls()) {
                // Certificado propio/autofirmado (red local): sin validar cadena ni nombre.
                // Tiene que ser X509ExtendedTrustManager: con X509TrustManager normal la JVM
                // lo envuelve y sigue comprobando el nombre del servidor.
                try {
                    SSLContext ctx = SSLContext.getInstance("TLS");
                    ctx.init(null, new TrustManager[] { new X509ExtendedTrustManager() {
                        public void checkClientTrusted(X509Certificate[] c, String a) { }
                        public void checkServerTrusted(X509Certificate[] c, String a) { }
                        public void checkClientTrusted(X509Certificate[] c, String a, Socket sk) { }
                        public void checkServerTrusted(X509Certificate[] c, String a, Socket sk) { }
                        public void checkClientTrusted(X509Certificate[] c, String a, SSLEngine e) { }
                        public void checkServerTrusted(X509Certificate[] c, String a, SSLEngine e) { }
                        public X509Certificate[] getAcceptedIssuers() { return new X509Certificate[0]; }
                    } }, new SecureRandom());
                    builder.sslContext(ctx);
                } catch (Exception e) {
                    throw new IllegalStateException("No se pudo crear el contexto TLS", e);
                }
            }
            http = builder.build();
        }
        return http;
    }

    private void resetSession() {
        token = null;
        siteId = null;
        prefix = null;
        http = null; // cookies nuevas
    }

    private static String text(JsonNode node, String field) {
        JsonNode v = node.path(field);
        return v.isMissingNode() || v.isNull() ? "" : v.asText("");
    }

    private static boolean blank(String s) {
        return s == null || s.isBlank();
    }

    private static class SessionException extends RuntimeException {
        SessionException(String message) {
            super(message);
        }
    }
}
