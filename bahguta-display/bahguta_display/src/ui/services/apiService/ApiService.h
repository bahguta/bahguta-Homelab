#pragma once

#include "../httpService/HttpService.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#include "../../models/ApiInfo.h"
#include "../../models/DeviceInfo.h"
#include "../../models/SensorInfo.h"
#include "../../models/WeatherInfo.h"
#include "../../models/DockerInfo.h"
#include "../../models/HomeInfo.h"
#include "../../models/BackendServerInfo.h"
#include "../../models/RedInfo.h"
#include "../../models/GrafanaInfo.h"
#include "../../models/SettingsInfo.h"
#include "../../models/CasaInfo.h"
#include "../../models/CasaHistory.h"

class ApiService
{
public:

    void init();

    // Una unica peticion REST (GET /api/v1/dashboard) cada refreshInterval.
    void update();

    bool connected() const;

    ApiInfo& api();

    HomeInfo& home();

    WeatherInfo& weather();

    DockerInfo& docker();

    DeviceInfo& devices();

    SensorInfo& sensors();

    BackendServerInfo& server();

    RedInfo& red();

    GrafanaInfo& grafana();

    SettingsInfo& settings();

    CasaInfo& casa();

    // Historico de mediciones por habitacion (grafica de CasaScreen)
    // requestCasaHistory(): pide una actualizacion (la hace la task de runtime, no la UI;
    // no toma el mutex, se puede llamar desde show()/update()).
    void requestCasaHistory();
    const CasaHistory& casaHistory() const { return casaHistoryData; }

    // Version bloqueante (POST inmediato). La UI usa requestSensorFrequency().
    bool setSensorFrequency(uint32_t seconds);

    // ------------------------------------------------------------
    // Cambios de configuracion pedidos desde la UI.
    // NUNCA bloquean LVGL: el valor se aplica al instante en la tablet
    // y el POST lo envia la task de runtime. Varios toques seguidos
    // se unen en un unico POST con el ultimo valor.
    // ------------------------------------------------------------

    enum class SaveState : uint8_t { Idle, Saving, Saved, Failed };

    // Backend: aplica varios ajustes de golpe (0 = no tocar ese). Un unico
    // POST con solo los campos indicados.
    void requestBackendSettings(uint32_t sensorSeconds, uint32_t weatherSeconds);

    // Horas de las franjas del pronostico (0-23); -1 = sin cambio en esa franja
    void requestForecastHours(int8_t hour1, int8_t hour2, int8_t hour3);

    // Backend: apiRefreshSeconds (cada cuanto miden los sensores)
    void requestSensorFrequency(uint32_t seconds);

    // Backend: weatherRefreshSeconds (cada cuanto renueva el tiempo)
    void requestWeatherRefresh(uint32_t seconds);

    // Local (NVS): cada cuanto consulta ESTA tablet al backend
    void setPollIntervalSeconds(uint32_t seconds);
    uint32_t pollIntervalSeconds() const;

    SaveState saveState() const;

    // Se incrementa cada vez que cambian los datos publicados (con el mutex).
    // Los servicios solo copian cuando cambia: antes copiaban estructuras
    // enormes con String cada 10 ms, tomando el mutex que necesita LVGL.
    uint32_t version() const { return dataVersion; }

    bool startContainer(const String& containerId);
    bool stopContainer(const String& containerId);

    bool startContainer(const char* containerId);
    bool stopContainer(const char* containerId);

    // Igual, pero SIN bloquear LVGL: la accion la envia la task de runtime
    // (un 'docker stop' puede tardar varios segundos y la UI se quedaba
    // congelada durante todo el POST). Despues se refresca el estado.
    void requestContainerAction(const char* containerId, bool start);

private:

    SensorInfo sensorsInfo;
    HomeInfo homeInfo;
    WeatherInfo weatherInfo;
    DockerInfo dockerInfo;
    DeviceInfo devicesInfo;
    ApiInfo apiInfo;
    BackendServerInfo serverInfo;
    RedInfo redInfo;
    GrafanaInfo grafanaInfo;
    SettingsInfo settingsInfo;
    CasaInfo casaInfo;

    CasaHistory casaHistoryData;       // lo que lee la UI (bajo mutex)
    CasaHistory historyScratch;        // donde se parsea fuera del mutex
    volatile bool historyWanted = false;
    void flushPendingHistory();

    HttpService http;

    uint32_t lastUpdate = 0;

    uint32_t refreshInterval = 30000;

    // Intervalo real hasta el siguiente intento: refreshInterval si todo
    // fue bien, RETRY_INTERVAL_MS (o menos) si la peticion fallo.
    uint32_t currentDelay = 30000;
    static constexpr uint32_t RETRY_INTERVAL_MS = 5000;

    // Version de la config que tiene la tablet (se envia como ?cfgVer=N).
    // Puede ser negativa (hash del backend) -> int32_t.
    int32_t cfgVersion = 0;
    bool haveCfgVersion = false;

    // Cambios pendientes de enviar (0 = ninguno)
    uint32_t dataVersion = 0;

    uint32_t pendingSensorSeconds = 0;
    uint32_t pendingWeatherSeconds = 0;
    int8_t pendingHour[3] = { -1, -1, -1 };   // franjas pendientes de enviar (-1 = ninguna)
    int8_t sentHour[3]    = { -1, -1, -1 };   // ultimas enviadas (para verificar)
    volatile SaveState saveStateValue = SaveState::Idle;
    uint32_t saveStateAt = 0;
    static constexpr uint32_t PENDING_TIMEOUT_MS = 15000;

    // Tras un POST correcto, la siguiente config del servidor debe devolver
    // lo enviado; si no, el servidor no lo esta guardando.
    bool verifySaved = false;
    uint32_t sentSensorSeconds = 0;
    uint32_t sentWeatherSeconds = 0;

    void flushPendingSettings();
    void flushPendingContainerAction();

    // Ultima accion pedida sobre un contenedor (solo se guarda la ultima)
    bool pendingContainer = false;
    bool pendingContainerStart = false;
    char pendingContainerId[40] = "";
    // Envia SOLO lo que el usuario cambio (0 = no enviar ese campo). El
    // backend trata lo que no llega como "sin cambios"; mandar el objeto
    // completo pisaba en la BBDD valores que la tablet tenia desfasados.
    bool postSettings(uint32_t sensorSeconds, uint32_t weatherSeconds, const int8_t* hours = nullptr);

    char sunriseBuffer[6] = "";
    char sunsetBuffer[6] = "";

    static constexpr const char* BASE_URL = "http://bahguta.local";
    static constexpr const char* DASHBOARD_URL = "/api/v1/dashboard/data";

    // Solo para ACCIONES (POST), ya no se usan para leer datos
    static constexpr const char* SETTINGS_URL = "/api/v1/settings";
    static constexpr const char* HISTORY_URL = "/api/v1/measurements/summary?hours=24&points=48";
    static constexpr const char* DOCKER_BASE_URL = "/api/v1/system/docker";

    String formatTime(long timestamp, long timezone);

    // true si la peticion y el JSON fueron correctos
    bool updateDashboard();

    // Todos los apply*() se llaman con el mutex de datos TOMADO.
    // Si la seccion llega con ok=false conservan el ultimo dato bueno.
    void applyWeather(JsonObject section);
    void applyRooms(JsonObject section);
    void applySummary(JsonObject summary);
    void applyDocker(JsonObject section);
    // Device + su sensor en una sola entrada (se fusionan por deviceId)
    void applyDevices(JsonObject devicesSection, JsonObject sensorsSection);
    void applySensors(JsonObject section);
    void applySystem(JsonObject section);
    void applyRed(JsonObject section);
    void applyGrafana(JsonObject section);
    void applyConfig(JsonObject config);
};
