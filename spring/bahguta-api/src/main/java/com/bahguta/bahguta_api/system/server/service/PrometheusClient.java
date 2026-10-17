package com.bahguta.bahguta_api.system.server.service;

import java.util.List;

public interface PrometheusClient {

    /** Valor de la primera serie; RuntimeException si no hay datos o falla. */
    double query(String query);

    /** Todas las series con sus etiquetas (lista vacia si no hay datos). */
    List<PromSample> queryVector(String query);
}
