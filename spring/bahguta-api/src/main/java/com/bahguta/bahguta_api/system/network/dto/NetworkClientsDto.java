package com.bahguta.bahguta_api.system.network.dto;

import java.util.List;

/** Equipos conectados a la red (leidos de Omada Controller). */
public record NetworkClientsDto(int total, List<Client> clients) {

    /** type: "wifi" | "cable" ("" si Omada no lo indica) */
    public record Client(String name, String ip, String mac, String type) {}
}
