package com.bahguta.bahguta_api.system.server.queries;

public final class PrometheusQueries {

    private PrometheusQueries() {
    }

    public static final String CPU =
        "100-(avg(irate(node_cpu_seconds_total{mode=\"idle\"}[5m]))*100)";

    public static final String RAM =
        "(1-(node_memory_MemAvailable_bytes/node_memory_MemTotal_bytes))*100";
    
    public static final String UPTIME = 
    	"time() - process_start_time_seconds";
}