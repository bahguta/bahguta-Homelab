#include <Preferences.h>
#include "ApiService.h"

#include <ArduinoJson.h>
#include <Arduino.h>
#include <string.h>
#include "../ServiceManager.h"
#include "../../secrets.h"
#include "../../utils/DateUtils.h"
#include <WiFi.h>
#include <HTTPClient.h>

// ============================================================
// DASHBOARD  --  UNICA peticion REST de lectura
//
//   GET /api/v1/dashboard[?cfgVer=N]
//
// Secciones: weather, rooms, summary, docker, devices, sensors,
// system y config (esta ultima solo si cambio en el backend).
// Timestamps en epoch SEGUNDOS.
// ============================================================

// Version del proyecto: ultima conocida, guardada en NVS
static void saveBackendVersion(const char* version)
{
    Preferences prefs;
    if (prefs.begin("bahguta_ver", false))
    {
        prefs.putString("ver", version);
        prefs.end();
    }
}

void ApiService::init()
{
    http.init();

    // Ultima version conocida del backend (para splash y footer antes de conectar)
    {
        Preferences prefs;
        if (prefs.begin("bahguta_ver", false))
        {
            String v = prefs.getString("ver", "");
            prefs.end();
            strlcpy(serverInfo.version, v.c_str(), sizeof(serverInfo.version));
        }
    }

    refreshInterval =
        ServiceManager::settings()
            .settings()
            .refreshInterval;

    currentDelay = refreshInterval;

    Serial.printf(
        "API refresh interval: %lu ms\n",
        refreshInterval
    );
}

void ApiService::update()
{
    // Cambios de configuracion y acciones de Docker pedidos desde la UI (POST fuera de LVGL)
    flushPendingSettings();
    flushPendingContainerAction();
    flushPendingHistory();

    // lastUpdate == 0 -> primera vez: pedimos ya, sin esperar un intervalo
    if (lastUpdate != 0 && millis() - lastUpdate < currentDelay)
        return;

    lastUpdate = millis();

    if (lastUpdate == 0)
        lastUpdate = 1;

    const uint32_t retry = RETRY_INTERVAL_MS;

    ServiceManager::lockData();
    const bool networkConnected =
        ServiceManager::network().network().connected;
    ServiceManager::unlockData();

    if (!networkConnected)
    {
        currentDelay = retry;
        return;
    }

    const bool ok = updateDashboard();

    if (ok)
    {
        currentDelay = refreshInterval;
    }
    else
    {
        // Reintento pronto, pero nunca mas lento que el intervalo normal
        currentDelay = (refreshInterval < retry) ? refreshInterval : retry;
    }
}

bool ApiService::updateDashboard()
{
    String url = String(BASE_URL) + DASHBOARD_URL;

    if (haveCfgVersion)
        url += "?cfgVer=" + String(cfgVersion);

    Serial.printf("DASHBOARD URL -> %s\n", url.c_str());

    // HTTP SIN mutex: la UI nunca espera a un timeout de red.
    HttpResponse response = http.get(url.c_str());

    if (!response.success || response.code != 200)
    {
        Serial.printf("DASHBOARD FAIL -> code=%d\n", response.code);

        ServiceManager::lockData();

        apiInfo.connected = false;
        homeInfo.success = false;

        // Sin backend tampoco hay docker ni metricas del servidor
        dockerInfo.connected = false;
        serverInfo.connected = false;
        redInfo.connected = false;
        grafanaInfo.connected = false;

        dataVersion++;
        ServiceManager::unlockData();
        return false;
    }

    // El parseo tambien va FUERA del mutex; solo se bloquea para copiar.
    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, response.body.c_str());

    if (error)
    {
        Serial.printf("DASHBOARD JSON ERROR: %s\n", error.c_str());

        ServiceManager::lockData();
        apiInfo.connected = false;
        homeInfo.success = false;
        dataVersion++;
        ServiceManager::unlockData();

        return false;
    }

    if (!(doc["success"] | false))
    {
        Serial.println("DASHBOARD: success=false");

        ServiceManager::lockData();
        apiInfo.connected = false;
        homeInfo.success = false;
        dataVersion++;
        ServiceManager::unlockData();

        return false;
    }

    JsonObject weatherSection = doc["weather"];
    JsonObject roomsSection   = doc["rooms"];
    JsonObject summary        = doc["summary"];
    JsonObject dockerSection  = doc["docker"];
    JsonObject devicesSection = doc["devices"];
    JsonObject sensorsSection = doc["sensors"];
    JsonObject systemSection  = doc["system"];
    JsonObject redSection     = doc["red"];
    JsonObject grafanaSection = doc["grafana"];
    JsonObject config         = doc["config"];

    // Version del proyecto (campo "version" del dashboard)
    const char* backendVersion = doc["version"] | "";
    char versionToSave[16] = "";

    ServiceManager::lockData();

    apiInfo.connected = true;
    homeInfo.success = true;

    if (backendVersion[0] && strcmp(backendVersion, serverInfo.version) != 0)
    {
        strlcpy(serverInfo.version, backendVersion, sizeof(serverInfo.version));
        strlcpy(versionToSave, backendVersion, sizeof(versionToSave));
    }

    applyWeather(weatherSection);
    applyRooms(roomsSection);
    applySummary(summary);
    applyDocker(dockerSection);
    applyDevices(devicesSection, sensorsSection);
    applySensors(sensorsSection);
    applySystem(systemSection);
    applyRed(redSection);
    applyGrafana(grafanaSection);

    // "config" solo viene si cambio: guardamos la version que ya tenemos
    if (!config.isNull())
    {
        applyConfig(config);

        // Si hay cambios del usuario aun sin enviar, mandan sobre lo que
        // diga el servidor (si no, se enviaria el valor viejo)
        if (pendingSensorSeconds != 0)
            settingsInfo.apiRefreshSeconds = pendingSensorSeconds;

        if (pendingWeatherSeconds != 0)
            settingsInfo.weatherRefreshSeconds = pendingWeatherSeconds;

        for (uint8_t i = 0; i < 3; i++)
            if (pendingHour[i] >= 0)
                settingsInfo.forecastHour[i] = (uint8_t)pendingHour[i];

        const bool hoursPending =
            pendingHour[0] >= 0 || pendingHour[1] >= 0 || pendingHour[2] >= 0;


        // Comprobacion tras guardar: lo que devuelve el servidor debe ser
        // lo que se envio
        if (verifySaved && pendingSensorSeconds == 0 && pendingWeatherSeconds == 0 && !hoursPending)
        {
            verifySaved = false;

            const bool sensorsDiffer =
                sentSensorSeconds != 0 &&
                settingsInfo.apiRefreshSeconds != sentSensorSeconds;

            const bool weatherDiffers =
                sentWeatherSeconds != 0 &&
                settingsInfo.weatherRefreshSeconds != sentWeatherSeconds;

            bool hoursDiffer = false;
            for (uint8_t i = 0; i < 3; i++)
                if (sentHour[i] >= 0 && settingsInfo.forecastHour[i] != (uint8_t)sentHour[i])
                    hoursDiffer = true;

            if (sensorsDiffer || weatherDiffers || hoursDiffer)
            {
                saveStateValue = SaveState::Failed;
                saveStateAt = millis();

                Serial.printf(
                    "[CONFIG] el servidor NO conserva el cambio: enviado sensores=%lu tiempo=%lu, devuelto sensores=%lu tiempo=%lu\n",
                    (unsigned long)sentSensorSeconds,
                    (unsigned long)sentWeatherSeconds,
                    (unsigned long)settingsInfo.apiRefreshSeconds,
                    (unsigned long)settingsInfo.weatherRefreshSeconds);
            }
        }

        if (doc["cfgVersion"].is<int32_t>())
        {
            cfgVersion = doc["cfgVersion"].as<int32_t>();
            haveCfgVersion = true;
        }
    }

    dataVersion++;
    ServiceManager::unlockData();

    // NVS fuera del mutex, y solo cuando cambia
    if (versionToSave[0])
        saveBackendVersion(versionToSave);

    Serial.printf(
        "DASHBOARD OK (%u bytes): weather=%d rooms=%d docker=%d devices=%d sensors=%d system=%d config=%s\n",
        (unsigned)response.body.length(),
        (int)(weatherSection["ok"] | false),
        (int)(roomsSection["ok"] | false),
        (int)(dockerSection["ok"] | false),
        (int)(devicesSection["ok"] | false),
        (int)(sensorsSection["ok"] | false),
        (int)(systemSection["ok"] | false),
        config.isNull() ? "sin cambios" : "actualizada"
    );

    return true;
}


// ------------------------------------------------------------
// WEATHER (actual + pronostico diario)
// ------------------------------------------------------------

void ApiService::applyWeather(JsonObject section)
{
    if (!(section["ok"] | false))
        return; // conservamos el ultimo dato bueno

    JsonObject data = section["data"];
    JsonObject cur  = data["current"];

    if (cur.isNull())
        return;

    weatherInfo.success = true;

    weatherInfo.location       = cur["location"] | "";
    weatherInfo.temperature    = cur["temperature"] | 0.0f;
    weatherInfo.feelsLike      = cur["feelsLike"] | 0.0f;
    weatherInfo.minTemperature = cur["minTemperature"] | 0.0f;
    weatherInfo.maxTemperature = cur["maxTemperature"] | 0.0f;
    weatherInfo.humidity       = cur["humidity"] | 0;
    weatherInfo.pressure       = cur["pressure"] | 0;
    weatherInfo.description    = cur["description"] | "";
    weatherInfo.icon           = cur["icon"] | "";
    weatherInfo.windSpeed      = cur["windSpeed"] | 0.0f;
    weatherInfo.windDirection  = cur["windDirection"] | 0;
    weatherInfo.clouds         = cur["clouds"] | 0;
    weatherInfo.timezone       = cur["timezone"] | 0;

    // La seccion trae updatedAt en SEGUNDOS (antes venia en ms)
    weatherInfo.updatedAt = section["updatedAt"] | 0;

    String sunriseTime =
        formatTime(cur["sunrise"] | 0L, weatherInfo.timezone);

    snprintf(sunriseBuffer, sizeof(sunriseBuffer), "%s", sunriseTime.c_str());
    weatherInfo.sunrise = sunriseBuffer;

    String sunsetTime =
        formatTime(cur["sunset"] | 0L, weatherInfo.timezone);

    snprintf(sunsetBuffer, sizeof(sunsetBuffer), "%s", sunsetTime.c_str());
    weatherInfo.sunset = sunsetBuffer;

    // ---- Pronostico: el backend ya manda UN dia por entrada ----
    JsonArray days = data["forecast"];

    // Si el pronostico vino vacio (fallo puntual del backend) no
    // borramos el que ya teniamos.
    if (days.isNull() || days.size() == 0)
        return;

    weatherInfo.forecastCount = 0;

    for (JsonObject day : days)
    {
        if (weatherInfo.forecastCount >= WeatherInfo::FORECAST_MAX_DAYS)
            break;

        const char* date = day["date"] | ""; // "YYYY-MM-DD"

        if (strlen(date) < 10)
            continue;

        int year =
            (date[0] - '0') * 1000 +
            (date[1] - '0') * 100 +
            (date[2] - '0') * 10 +
            (date[3] - '0');

        int month = (date[5] - '0') * 10 + (date[6] - '0');
        int dd    = (date[8] - '0') * 10 + (date[9] - '0');

        uint8_t wday = DateUtils::weekdayFromDate(year, month, dd);

        WeatherInfo::ForecastEntry& entry =
            weatherInfo.forecast[weatherInfo.forecastCount];

        snprintf(
            entry.label,
            sizeof(entry.label),
            "%s\n%02d/%02d",
            DateUtils::weekday(wday),
            dd,
            month);

        entry.label[sizeof(entry.label) - 1] = '\0';

        const char* icon = day["icon"] | "";

        strncpy(entry.icon, icon, sizeof(entry.icon) - 1);
        entry.icon[sizeof(entry.icon) - 1] = '\0';

        // El backend manda minima y maxima del dia
        entry.maxTemperature = day["max"] | 0.0f;
        entry.minTemperature = day["min"] | 0.0f;
        entry.temperature = entry.maxTemperature;

        // Franjas del dia (si el backend aun no las envia, hourCount = 0)
        entry.hourCount = 0;

        JsonArray hours = day["hours"];

        for (JsonObject h : hours)
        {
            if (entry.hourCount >= WeatherInfo::FORECAST_HOURS)
                break;

            WeatherInfo::ForecastHour& slot = entry.hours[entry.hourCount];

            slot.hour = (uint8_t)(h["hour"] | 0);
            slot.minTemperature = h["min"] | 0.0f;
            slot.maxTemperature = h["max"] | 0.0f;

            const char* hourIcon = h["icon"] | "";

            strncpy(slot.icon, hourIcon, sizeof(slot.icon) - 1);
            slot.icon[sizeof(slot.icon) - 1] = '\0';

            entry.hourCount++;
        }

        weatherInfo.forecastCount++;
    }
}


// ------------------------------------------------------------
// CASA (una entrada por habitacion; el backend ya filtra la
// ultima medicion y marca "stale")
// ------------------------------------------------------------

void ApiService::applyRooms(JsonObject section)
{
    if (!(section["ok"] | false))
        return;

    JsonArray rooms = section["data"];

    casaInfo.success = true;
    casaInfo.roomCount = 0;

    for (JsonObject room : rooms)
    {
        if (casaInfo.roomCount >= 12)
            break;

        RoomInfo& r = casaInfo.rooms[casaInfo.roomCount];

        const char* name = room["room"] | "";

        r.name = name;
        r.id = r.name;
        r.lastMeasurement = String((unsigned long)(room["lastMeasurement"] | 0L));

        const bool stale = room["stale"] | false;

        double tempVal = room["temperature"] | 0.0;
        double humVal  = room["humidity"] | 0.0;

        // Se muestra SIEMPRE la ultima medicion conocida (si esta vieja,
        // la pantalla la atenua con r.stale). Solo se descarta si no hay
        // dato o esta fuera de rango del DHT22 (125.0 / -40.0 = lectura fallida).
        r.stale = stale;

        r.temperatureAvailable =
            room["temperature"].is<double>() &&
            tempVal > -20.0 && tempVal < 60.0;

        r.temperature = r.temperatureAvailable ? tempVal : 0.0;

        r.humidityAvailable =
            room["humidity"].is<double>() &&
            humVal >= 0.0 && humVal <= 100.0;

        r.humidity = r.humidityAvailable ? humVal : 0.0;

        casaInfo.roomCount++;
    }

    double sum = 0.0;
    int count = 0;

    for (int i = 0; i < casaInfo.roomCount; i++)
    {
        if (casaInfo.rooms[i].temperatureAvailable)
        {
            sum += casaInfo.rooms[i].temperature;
            count++;
        }
    }

    casaInfo.temperatureAvailable = count > 0;
    casaInfo.averageTemperature = (count > 0) ? (sum / count) : 0.0;

    double humSum = 0.0;
    int humCount = 0;

    for (int i = 0; i < casaInfo.roomCount; i++)
    {
        if (casaInfo.rooms[i].humidityAvailable)
        {
            humSum += casaInfo.rooms[i].humidity;
            humCount++;
        }
    }

    casaInfo.humidityAvailable = humCount > 0;
    casaInfo.averageHumidity = (humCount > 0) ? (humSum / humCount) : 0.0;

    casaInfo.updatedAt = section["updatedAt"] | 0;
}


// ------------------------------------------------------------
// SUMMARY (totales de las cards de Home)
// ------------------------------------------------------------

void ApiService::applySummary(JsonObject summary)
{
    JsonObject devs = summary["devices"];

    if (!devs.isNull())
    {
        int total  = devs["total"] | 0;
        int online = devs["online"] | 0;

        homeInfo.devices.total   = total;
        homeInfo.devices.online  = online;
        homeInfo.devices.offline = total - online;

        devicesInfo.total   = total;
        devicesInfo.online  = online;
        devicesInfo.offline = total - online;
    }

    JsonObject sens = summary["sensors"];

    if (!sens.isNull())
    {
        int total  = sens["total"] | 0;
        int online = sens["online"] | 0;

        homeInfo.sensors.total   = total;
        homeInfo.sensors.online  = online;
        homeInfo.sensors.offline = total - online;

        sensorsInfo.total   = total;
        sensorsInfo.online  = online;
        sensorsInfo.offline = total - online;
    }
}


// ------------------------------------------------------------
// DOCKER (id, name, state, status -- image/ports ya no viajan)
// ------------------------------------------------------------

void ApiService::applyDocker(JsonObject section)
{
    if (!(section["ok"] | false))
    {
        dockerInfo.online = 0;
        dockerInfo.total = 0;
        dockerInfo.connected = false;
        return;
    }

    JsonArray containers = section["data"];

    int total = 0;
    int online = 0;

    dockerInfo.containerCount = 0;

    for (JsonObject container : containers)
    {
        if (dockerInfo.containerCount >= 32)
            break;

        DockerContainerInfo& info =
            dockerInfo.containers[dockerInfo.containerCount];

        info.id     = container["id"] | "";
        info.name   = container["name"] | "";
        info.status = container["status"] | "";
        info.state  = container["state"] | "";
        info.image  = "";
        info.ports  = "";

        dockerInfo.containerCount++;
        total++;

        if (info.state == "running")
            online++;
    }

    dockerInfo.total = total;
    dockerInfo.online = online;
    dockerInfo.connected = true;
}


// ------------------------------------------------------------
// DEVICES + SENSOR (una sola entrada por device)
//
// El backend envia devices[] y sensors[] por separado; en la tablet
// son lo mismo (placa ESP32-C3 + DHT22), asi que se fusionan por
// deviceId y la pantalla Devices muestra todo en una card.
// ------------------------------------------------------------

void ApiService::applyDevices(JsonObject devicesSection, JsonObject sensorsSection)
{
    if (!(devicesSection["ok"] | false))
        return;

    JsonArray items = devicesSection["data"];

    const bool sensorsOk = sensorsSection["ok"] | false;
    JsonArray sensorItems = sensorsSection["data"];

    devicesInfo.deviceListCount = 0;

    for (JsonObject item : items)
    {
        if (devicesInfo.deviceListCount >= 5)
            break;

        DeviceEntryInfo& entry =
            devicesInfo.devicesList[devicesInfo.deviceListCount];

        const char* id = item["id"] | "";

        entry.id            = id;
        entry.name          = item["name"] | "";
        entry.description   = item["description"] | "";
        entry.hardwareModel = item["hardwareModel"] | "";
        entry.location      = item["location"] | "";
        entry.deviceType    = item["deviceType"] | "";
        entry.ipAddress     = item["ip"] | "";
        entry.macAddress    = item["mac"] | "";
        entry.firmwareVersion = item["fw"] | "";

        String status = item["status"] | "";
        entry.online = (status == "ONLINE");

        // ---- sensor de este device (el primero que coincida) ----
        entry.hasSensor = false;
        entry.sensorId = "";
        entry.sensorType = "";
        entry.sensorEnabled = false;
        entry.sensorInterval = 0;
        entry.hasReading = false;
        entry.humidityValid = false;
        entry.temperature = 0.0f;
        entry.humidity = 0.0f;
        entry.stale = true;

        if (sensorsOk)
        {
            for (JsonObject s : sensorItems)
            {
                const char* deviceId = s["deviceId"] | "";

                if (strcmp(deviceId, id) != 0)
                    continue;

                entry.hasSensor = true;
                entry.sensorId = s["id"] | "";
                entry.sensorType = s["type"] | "";
                entry.sensorEnabled = s["enabled"] | false;
                entry.sensorInterval = s["interval"] | 0;
                entry.stale = s["stale"] | true;

                // Solo valores plausibles (el DHT22 devuelve 125 / -40
                // cuando falla la lectura)
                const double t = s["temperature"] | 0.0;
                const double h = s["humidity"] | 0.0;

                entry.hasReading =
                    s["temperature"].is<double>() &&
                    t > -20.0 && t < 60.0;

                entry.humidityValid =
                    s["humidity"].is<double>() &&
                    h >= 0.0 && h <= 100.0;

                entry.temperature = entry.hasReading ? (float)t : 0.0f;
                entry.humidity = entry.humidityValid ? (float)h : 0.0f;

                break;
            }
        }

        devicesInfo.deviceListCount++;
    }
}


// ------------------------------------------------------------
// SENSORS (config + ultima medicion, campos planos)
// ------------------------------------------------------------

void ApiService::applySensors(JsonObject section)
{
    if (!(section["ok"] | false))
        return;

    JsonArray items = section["data"];

    sensorsInfo.sensorListCount = 0;

    for (JsonObject item : items)
    {
        if (sensorsInfo.sensorListCount >= 5)
            break;

        SensorEntryInfo& entry =
            sensorsInfo.sensorsList[sensorsInfo.sensorListCount];

        entry.id       = item["id"] | "";
        entry.deviceId = item["deviceId"] | "";
        entry.name     = item["name"] | "";
        entry.type     = item["type"] | "";
        entry.enabled  = item["enabled"] | false;
        entry.interval = item["interval"] | 0;

        sensorsInfo.sensorListCount++;
    }
}


// ------------------------------------------------------------
// SYSTEM (cpu / ram / uptime del servidor)
// ------------------------------------------------------------

void ApiService::applySystem(JsonObject section)
{
    if (!(section["ok"] | false))
    {
        serverInfo.connected = false;
        return;
    }

    JsonObject data = section["data"];

    serverInfo.cpu    = data["cpu"] | 0.0;
    serverInfo.ram    = data["ram"] | 0.0;
    serverInfo.uptime = data["uptime"] | 0.0;
    serverInfo.connected = true;
}


// ------------------------------------------------------------
// CONFIG (solo llega cuando cambia en el backend)
// ------------------------------------------------------------

void ApiService::applyConfig(JsonObject data)
{
    settingsInfo.apiRefreshSeconds =
        data["apiRefreshSeconds"] | settingsInfo.apiRefreshSeconds;

    settingsInfo.weatherRefreshSeconds =
        data["weatherRefreshSeconds"] | settingsInfo.weatherRefreshSeconds;

    settingsInfo.systemRefreshSeconds =
        data["systemRefreshSeconds"] | settingsInfo.systemRefreshSeconds;

    settingsInfo.mqttReconnectSeconds =
        data["mqttReconnectSeconds"] | settingsInfo.mqttReconnectSeconds;

    settingsInfo.deviceName =
        (const char*)(data["deviceName"] | settingsInfo.deviceName.c_str());

    settingsInfo.deviceLocation =
        (const char*)(data["deviceLocation"] | settingsInfo.deviceLocation.c_str());

    settingsInfo.brightness =
        data["brightness"] | settingsInfo.brightness;

    settingsInfo.screenAlwaysOn =
        data["screenAlwaysOn"] | settingsInfo.screenAlwaysOn;

    settingsInfo.screenTimeoutSeconds =
        data["screenTimeoutSeconds"] | settingsInfo.screenTimeoutSeconds;

    for (uint8_t i = 0; i < 3; i++)
    {
        char key[16];
        snprintf(key, sizeof(key), "forecastHour%u", (unsigned)(i + 1));

        const int h = data[key] | (int)settingsInfo.forecastHour[i];
        if (h >= 0 && h <= 23)
            settingsInfo.forecastHour[i] = (uint8_t)h;
    }

    settingsInfo.configured = true;
    Serial.printf(
        "[CONFIG] recibida del servidor: sensores=%lu s, tiempo=%lu s, sistema=%lu s\n",
        (unsigned long)settingsInfo.apiRefreshSeconds,
        (unsigned long)settingsInfo.weatherRefreshSeconds,
        (unsigned long)settingsInfo.systemRefreshSeconds);
}


bool ApiService::connected() const
{
    return apiInfo.connected;
}

ApiInfo& ApiService::api(){
    return apiInfo;
}



String ApiService::formatTime(long timestamp, long timezone)
{
    if (timestamp <= 0) {
        return "--:--";
    }

    time_t localTimestamp = timestamp + timezone;

    struct tm* timeinfo = gmtime(&localTimestamp);

    char buffer[6];
    strftime(buffer, sizeof(buffer), "%H:%M", timeinfo);

    return String(buffer);
}






HomeInfo& ApiService::home()
{
    return homeInfo;
}

WeatherInfo& ApiService::weather()
{
    return weatherInfo;
}

DockerInfo& ApiService::docker()
{
    return dockerInfo;
}

DeviceInfo& ApiService::devices()
{
    return devicesInfo;
}

SensorInfo& ApiService::sensors()
{
    return sensorsInfo;
}

RedInfo& ApiService::red()
{
    return redInfo;
}

GrafanaInfo& ApiService::grafana()
{
    return grafanaInfo;
}

BackendServerInfo& ApiService::server()
{
    return serverInfo;
}

SettingsInfo& ApiService::settings()
{
    return settingsInfo;
}


/*
 * Actualizacion "optimista": mandamos el cambio al backend y,
 * si responde 200 OK, actualizamos tambien el valor en local
 * para que la UI lo refleje al instante sin esperar al
 * siguiente ciclo de updateSettings().
 *
 * IMPORTANTE: el backend debe exponer:
 *
 *   POST /api/v1/settings
 *   Body: {"apiRefreshSeconds": <segundos>}
 *   Respuesta esperada: {"success": true}
 */

bool ApiService::postSettings(uint32_t sensorSeconds, uint32_t weatherSeconds, const int8_t* hours)
{
    // Cuerpo parcial: solo los campos que cambiaron
    JsonDocument doc;

    if (sensorSeconds != 0)
        doc["apiRefreshSeconds"] = sensorSeconds;

    if (weatherSeconds != 0)
        doc["weatherRefreshSeconds"] = weatherSeconds;

    if (hours)
    {
        static const char* const KEYS[3] = { "forecastHour1", "forecastHour2", "forecastHour3" };
        for (uint8_t i = 0; i < 3; i++)
            if (hours[i] >= 0)
                doc[KEYS[i]] = (int)hours[i];
    }


    if (doc.size() == 0)
        return true;    // nada que enviar

    String body;
    serializeJson(doc, body);

    String url = String(BASE_URL) + SETTINGS_URL;

    Serial.printf(
        "SETTINGS UPDATE -------- %s body=%s\n",
        url.c_str(),
        body.c_str()
    );

    HttpResponse response =
        http.post(url.c_str(), body.c_str());

    if (!response.success || response.code != 200)
    {
        Serial.printf(
            "SETTINGS UPDATE FAIL --- code=%d body=%s\n",
            response.code,
            response.body.c_str()
        );

        return false;
    }

    return true;
}

bool ApiService::setSensorFrequency(uint32_t seconds)
{
    ServiceManager::lockData();
    settingsInfo.apiRefreshSeconds = seconds;
    ServiceManager::unlockData();

    return postSettings(seconds, 0);
}


// ------------------------------------------------------------
// Cambios de configuracion pedidos desde la UI
// ------------------------------------------------------------

void ApiService::requestBackendSettings(uint32_t sensorSeconds, uint32_t weatherSeconds)
{
    if (sensorSeconds == 0 && weatherSeconds == 0)
        return;

    ServiceManager::lockData();

    // Optimista: la UI ve ya el valor; el POST lo envia la task de runtime
    if (sensorSeconds != 0)
    {
        settingsInfo.apiRefreshSeconds = sensorSeconds;
        pendingSensorSeconds = sensorSeconds;
    }

    if (weatherSeconds != 0)
    {
        settingsInfo.weatherRefreshSeconds = weatherSeconds;
        pendingWeatherSeconds = weatherSeconds;
    }

    saveStateValue = SaveState::Saving;
    saveStateAt = millis();

    ServiceManager::unlockData();
}

void ApiService::requestForecastHours(int8_t hour1, int8_t hour2, int8_t hour3)
{
    const int8_t hours[3] = { hour1, hour2, hour3 };

    bool any = false;
    for (uint8_t i = 0; i < 3; i++)
        any = any || (hours[i] >= 0 && hours[i] <= 23);
    if (!any)
        return;

    ServiceManager::lockData();

    // Optimista: la UI ve ya el valor; el POST lo envia la task de runtime
    for (uint8_t i = 0; i < 3; i++)
    {
        if (hours[i] >= 0 && hours[i] <= 23)
        {
            settingsInfo.forecastHour[i] = (uint8_t)hours[i];
            pendingHour[i] = hours[i];
        }
    }

    saveStateValue = SaveState::Saving;
    saveStateAt = millis();

    ServiceManager::unlockData();
}


void ApiService::requestSensorFrequency(uint32_t seconds)
{
    requestBackendSettings(seconds, 0);
}

void ApiService::requestWeatherRefresh(uint32_t seconds)
{
    requestBackendSettings(0, seconds);
}

void ApiService::setPollIntervalSeconds(uint32_t seconds)
{
    if (seconds < 5)    seconds = 5;
    if (seconds > 3600) seconds = 3600;

    const uint32_t ms = seconds * 1000UL;

    ServiceManager::lockData();

    refreshInterval = ms;

    // Si el intervalo baja, el proximo poll no espera el valor antiguo
    if (currentDelay > ms)
        currentDelay = ms;

    ServiceManager::unlockData();

    // Persistir en NVS (tambien lo lee ApiService::init en el arranque)
    ServiceManager::settings().settings().refreshInterval = ms;
    ServiceManager::settings().save();

    Serial.printf("POLL INTERVAL -> %lu s (NVS)\n", (unsigned long)seconds);
}

uint32_t ApiService::pollIntervalSeconds() const
{
    return refreshInterval / 1000UL;
}

ApiService::SaveState ApiService::saveState() const
{
    return saveStateValue;
}

void ApiService::flushPendingSettings()
{
    ServiceManager::lockData();

    const uint32_t sensorSec  = pendingSensorSeconds;
    const uint32_t weatherSec = pendingWeatherSeconds;
    const int8_t hoursToSend[3] = { pendingHour[0], pendingHour[1], pendingHour[2] };
    const bool hasHours = hoursToSend[0] >= 0 || hoursToSend[1] >= 0 || hoursToSend[2] >= 0;

    if (sensorSec == 0 && weatherSec == 0 && !hasHours)
    {
        ServiceManager::unlockData();
        return;
    }

    const bool networkConnected =
        ServiceManager::network().network().connected;

    if (!networkConnected)
    {
        // Sin red se mantiene pendiente un rato; si no vuelve, fallo
        if (millis() - saveStateAt > PENDING_TIMEOUT_MS)
        {
            pendingSensorSeconds = 0;
            pendingWeatherSeconds = 0;
            for (uint8_t i = 0; i < 3; i++) pendingHour[i] = -1;
            saveStateValue = SaveState::Failed;
            saveStateAt = millis();
            haveCfgVersion = false; // resincroniza con el backend al volver
        }

        ServiceManager::unlockData();
        return;
    }

    pendingSensorSeconds = 0;
    pendingWeatherSeconds = 0;
    for (uint8_t i = 0; i < 3; i++) pendingHour[i] = -1;

    ServiceManager::unlockData();

    // Un unico POST con SOLO lo que cambio. Sin mutex: la UI nunca espera a la red.
    const bool ok = postSettings(sensorSec, weatherSec, hoursToSend);

    ServiceManager::lockData();

    saveStateValue = ok ? SaveState::Saved : SaveState::Failed;
    saveStateAt = millis();

    // La comprobacion posterior solo mira los campos que se enviaron
    verifySaved = ok;
    sentSensorSeconds = sensorSec;
    sentWeatherSeconds = weatherSec;
    for (uint8_t i = 0; i < 3; i++) sentHour[i] = hoursToSend[i];


    // Pedimos la config real al backend en el siguiente ciclo (sin cfgVer):
    // confirma el cambio o revierte el valor optimista de la UI.
    haveCfgVersion = false;
    lastUpdate = 0;

    ServiceManager::unlockData();
}



CasaInfo& ApiService::casa()
{
    return casaInfo;
}


// ============================================================
// DOCKER CONTAINER CONTROL
// ============================================================

static bool postContainerAction(
    HttpService& http,
    const char* baseUrl,
    const char* containerId,
    const char* action,
    const char* apiKey = nullptr
)
{
    String url =
        String(baseUrl) +
        "/api/v1/system/docker/container/" +
        String(containerId) +
        "/" +
        String(action);

    Serial.printf(
        "DOCKER ACTION -------- %s\n",
        url.c_str()
    );

    HttpResponse response = http.post(url.c_str(), "", BAHGUTA_API_KEY);

    if (!response.success || response.code != 200)
    {
        Serial.printf(
            "DOCKER ACTION FAIL --- code=%d\n",
            response.code
        );

        return false;
    }

    return true;
}


bool ApiService::startContainer(const char* containerId)
{
    return postContainerAction(http, BASE_URL, containerId, "start");
}


bool ApiService::stopContainer(const char* containerId)
{
    return postContainerAction(http, BASE_URL, containerId, "stop");
}


// ============================================================
// DOCKER CONTAINER CONTROL
// ============================================================

/*
 * "Optimista": si el backend responde 200, asumimos que el
 * comando se lanzo bien. El siguiente ciclo de updateDocker()
 * (cada 30s) confirmara el estado real -- no forzamos un
 * refresco inmediato para no complicar la sincronizacion con
 * el toggle que el usuario acaba de tocar.
 */

void ApiService::requestContainerAction(const char* containerId, bool start)
{
    if (containerId == nullptr || containerId[0] == '\0')
        return;

    ServiceManager::lockData();

    snprintf(pendingContainerId, sizeof(pendingContainerId), "%s", containerId);
    pendingContainerStart = start;
    pendingContainer = true;

    ServiceManager::unlockData();
}

void ApiService::flushPendingContainerAction()
{
    char id[sizeof(pendingContainerId)];
    bool start;

    ServiceManager::lockData();

    if (!pendingContainer)
    {
        ServiceManager::unlockData();
        return;
    }

    snprintf(id, sizeof(id), "%s", pendingContainerId);
    start = pendingContainerStart;
    pendingContainer = false;

    const bool networkConnected =
        ServiceManager::network().network().connected;

    ServiceManager::unlockData();

    if (networkConnected)
    {
        // POST sin mutex: la UI nunca espera a la red
        const bool ok = start ? startContainer(id) : stopContainer(id);

        Serial.printf("DOCKER %s %s -> %s\n", start ? "start" : "stop", id, ok ? "OK" : "FALLO");
    }

    // Pida lo que pida, se refresca el estado real enseguida (si la accion
    // fallo, el switch vuelve a su sitio con ese dato)
    ServiceManager::lockData();
    lastUpdate = 0;
    ServiceManager::unlockData();
}


bool ApiService::startContainer(const String& containerId)
{
    String url =
        String(BASE_URL) +
        DOCKER_BASE_URL +
        "/container/" +
        containerId +
        "/start";

    Serial.printf(
        "DOCKER START -------- %s\n",
        url.c_str()
    );

    HttpResponse response = http.post(url.c_str(), "", BAHGUTA_API_KEY);

    if (!response.success || response.code != 200)
    {
        Serial.printf(
            "DOCKER START FAIL --- code=%d\n",
            response.code
        );

        return false;
    }

    return true;
}


bool ApiService::stopContainer(const String& containerId)
{
    String url =
        String(BASE_URL) +
        DOCKER_BASE_URL +
        "/container/" +
        containerId +
        "/stop";

    Serial.printf(
        "DOCKER STOP -------- %s\n",
        url.c_str()
    );

    HttpResponse response = http.post(url.c_str(), "", BAHGUTA_API_KEY);

    if (!response.success || response.code != 200)
    {
        Serial.printf(
            "DOCKER STOP FAIL --- code=%d\n",
            response.code
        );

        return false;
    }

    return true;
}


// ------------------------------------------------------------
// RED: equipos conectados al router
//   "red": { ok, data: { total, clients: [ {name, ip, mac, type} ] } }
// ------------------------------------------------------------

void ApiService::applyRed(JsonObject section)
{
    if (!(section["ok"] | false))
    {
        // Conserva la ultima lista buena, solo marca que no hay conexion
        redInfo.connected = false;
        return;
    }

    JsonObject data = section["data"];
    JsonArray clients = data["clients"];

    redInfo.clients.clear();

    for (JsonObject c : clients)
    {
        if (redInfo.clients.size() >= 40)
            break;

        RedClientInfo info;

        info.name       = c["name"] | "";
        info.ip         = c["ip"] | "";
        info.mac        = c["mac"] | "";
        info.connection = c["type"] | "";

        redInfo.clients.push_back(info);
    }

    redInfo.total = data["total"] | (int)redInfo.clients.size();
    redInfo.connected = true;
}


// ------------------------------------------------------------
// GRAFANA: metricas del servidor (-1 = dato no disponible)
//   "grafana": { ok, data: { rootTotalGb, rootUsedGb, rootFreeGb,
//     storageTotalGb, storageUsedGb, storageFreeGb, cpu, ram, tempC } }
// ------------------------------------------------------------

void ApiService::applyGrafana(JsonObject section)
{
    if (!(section["ok"] | false))
    {
        grafanaInfo.connected = false;
        return;
    }

    JsonObject d = section["data"];

    grafanaInfo.rootTotalGb    = d["rootTotalGb"]    | -1.0;
    grafanaInfo.rootUsedGb     = d["rootUsedGb"]     | -1.0;
    grafanaInfo.rootFreeGb     = d["rootFreeGb"]     | -1.0;

    grafanaInfo.storageTotalGb = d["storageTotalGb"] | -1.0;
    grafanaInfo.storageUsedGb  = d["storageUsedGb"]  | -1.0;
    grafanaInfo.storageFreeGb  = d["storageFreeGb"]  | -1.0;

    grafanaInfo.cpu   = d["cpu"]   | -1.0;
    grafanaInfo.ram   = d["ram"]   | -1.0;
    grafanaInfo.tempC = d["tempC"] | -1.0;

    grafanaInfo.connected = true;
}

// ------------------------------------------------------------
// HISTORICO (grafica de CasaScreen)
//   GET /api/v1/measurements/summary?hours=24&points=24
//   data: { from, to, stepSeconds, rooms: [ { room, sensorId, points: [ {ts, t, h} ] } ] }
// ------------------------------------------------------------

void ApiService::requestCasaHistory()
{
    // Sin mutex: se llama desde la UI con el mutex ya tomado; solo levanta una bandera
    historyWanted = true;
}


void ApiService::flushPendingHistory()
{
    if (!historyWanted)
        return;

    ServiceManager::lockData();
    const bool networkConnected =
        ServiceManager::network().network().connected;
    ServiceManager::unlockData();

    if (!networkConnected)
        return;                       // se queda pendiente hasta que haya red

    historyWanted = false;

    String url = String(BASE_URL) + HISTORY_URL;

    // HTTP y parseo FUERA del mutex, como el dashboard
    HttpResponse response = http.get(url.c_str());

    if (!response.success || response.code != 200)
    {
        Serial.printf("HISTORY FAIL -> code=%d\n", response.code);
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, response.body.c_str());

    if (error || !(doc["success"] | false))
    {
        Serial.printf("HISTORY JSON ERROR: %s\n", error ? error.c_str() : "success=false");
        return;
    }

    JsonObject data = doc["data"];
    JsonArray rooms = data["rooms"];

    CasaHistory& h = historyScratch;

    h.available = true;
    h.from = data["from"] | 0u;
    h.to = data["to"] | 0u;
    h.stepSeconds = data["stepSeconds"] | 3600u;
    h.seriesCount = 0;

    for (JsonObject room : rooms)
    {
        if (h.seriesCount >= CasaHistory::MAX_SERIES)
            break;

        HistorySeries& series = h.series[h.seriesCount];

        strlcpy(series.room, room["room"] | "", sizeof(series.room));
        series.count = 0;

        for (JsonObject p : room["points"].as<JsonArray>())
        {
            if (series.count >= HistorySeries::MAX_POINTS)
                break;

            HistoryPoint& pt = series.points[series.count];

            pt.ts = p["ts"] | 0u;
            pt.t = p["t"].isNull() ? NAN : p["t"].as<float>();
            pt.h = p["h"].isNull() ? NAN : p["h"].as<float>();

            series.count++;
        }

        h.seriesCount++;
    }

    ServiceManager::lockData();

    h.version = casaHistoryData.version + 1;
    casaHistoryData = h;
    dataVersion++;

    ServiceManager::unlockData();

    Serial.printf("HISTORY OK -> %u series\n", (unsigned)h.seriesCount);
}
