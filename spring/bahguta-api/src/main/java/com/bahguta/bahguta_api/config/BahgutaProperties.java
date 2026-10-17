package com.bahguta.bahguta_api.config;

import org.springframework.boot.context.properties.ConfigurationProperties;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
@ConfigurationProperties(prefix = "bahguta")
public class BahgutaProperties {

    public Integer heartbeatSeconds;
    public Mqtt mqtt;
    public Scripts scripts;
    public Weather weather;
    public Prometheus prometheus;
    public Grafana grafana;
    public Omada omada;
    

    @Getter
    @Setter
    public static class Prometheus {
        private String url;
    }

    @Getter
    @Setter
    public static class Grafana {
        /** Punto de montaje del disco del sistema (por defecto /) */
        private String rootMount;
        /** Trozo del nombre de device del disco del sistema (por defecto ubuntu--lv) */
        private String rootDevice;
        /** Punto de montaje del disco de datos (por defecto /srv/storage) */
        private String storageMount;
        /** Trozo del nombre de device del disco de datos (por defecto sda1) */
        private String storageDevice;
        /** Query de temperatura (opcional; por defecto node_hwmon_temp_celsius) */
        private String tempQuery;
    }

    @Getter
    @Setter
    public static class Omada {
        private String url;
        private String username;
        private String password;
        private String site;
        /** true si el certificado es autofirmado / no valido para la JVM */
        private boolean insecureTls;
    }

    @Getter
    @Setter
    public static class Mqtt {
        public String host;
        public Integer port;
        public String topic;
    }

    @Getter
    @Setter
    public static class Scripts {
        public Docker docker;
        public String path;

        @Getter
        @Setter
        public static class Docker {
            public String list;
            public String start;
            public String stop;
        }
    }

    @Getter
    @Setter
    public static class Weather {
        public String apiKey;
        public String city;
        public String country;
    }
}