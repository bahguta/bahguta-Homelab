#include "ConfigScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../services/ServiceManager.h"

#include <Arduino.h>
#include <string.h>
#include <stdio.h>

/*
 * Valores permitidos para cada ajuste (segundos). Los botones -/+
 * saltan entre ellos, asi que nunca se llega a un valor absurdo.
 */
static constexpr uint32_t SENSOR_PRESETS[] =
    { 10, 30, 60, 120, 300, 600, 900, 1800, 3600 };

static constexpr uint32_t POLL_PRESETS[] =
    { 5, 10, 15, 30, 60, 120, 300 };

static constexpr uint32_t WEATHER_PRESETS[] =
    { 300, 600, 900, 1800, 3600 };

// Apagado de pantalla (local). 86400 es solo el valor "Nunca" de la interfaz;
// en la NVS se guarda 0 (= no apagar nunca).
static constexpr uint32_t SCREEN_NEVER = 86400;

static constexpr uint32_t SCREEN_PRESETS[] =
    { 30, 60, 120, 300, 600, 900, 1800, 3600, SCREEN_NEVER };

static uint32_t screenUiValue(uint32_t seconds)      // NVS -> interfaz
{
    return (seconds == 0 || seconds >= SCREEN_NEVER) ? SCREEN_NEVER : seconds;
}

static uint32_t screenStoredValue(uint32_t uiSeconds) // interfaz -> NVS
{
    return (uiSeconds >= SCREEN_NEVER) ? 0 : uiSeconds;
}

template <size_t N>
static uint32_t nextPreset(const uint32_t (&presets)[N], uint32_t current, int dir)
{
    if (dir > 0)
    {
        // primer preset mayor que el actual
        for (size_t i = 0; i < N; i++)
            if (presets[i] > current)
                return presets[i];

        return presets[N - 1];
    }

    // ultimo preset menor que el actual
    for (size_t i = N; i > 0; i--)
        if (presets[i - 1] < current)
            return presets[i - 1];

    return presets[0];
}

static void formatSeconds(uint32_t s, char* buf, size_t size)
{
    if (s >= SCREEN_NEVER)
    {
        snprintf(buf, size, "Nunca");
        return;
    }

    if (s < 60)
    {
        snprintf(buf, size, "%u s", (unsigned)s);
    }
    else if (s < 3600)
    {
        if (s % 60 == 0)
            snprintf(buf, size, "%u min", (unsigned)(s / 60));
        else
            snprintf(buf, size, "%u s", (unsigned)s);
    }
    else
    {
        if (s % 3600 == 0)
            snprintf(buf, size, "%u h", (unsigned)(s / 3600));
        else
            snprintf(buf, size, "%u min", (unsigned)(s / 60));
    }
}



// ------------------------------------------------------------
// Geometria (area de contenido 800 x 970)
// ------------------------------------------------------------
static constexpr int PANEL_TOP = ContentLayout::CONTENT_TOP;
static constexpr int PANEL_PAD = 10;
static constexpr int INFO_ROW_H = 30;
static constexpr int STEPPER_TOP = PANEL_TOP + 274;
static constexpr int STEPPER_PITCH = 98;    // compacto: caben 4 intervalos + la fila de franjas
// Boton de actualizar anclado al FONDO de la pantalla (como apagar/reiniciar en Settings)
static constexpr int APPLY_H = 64;
static constexpr int APPLY_MARGIN = 20;                      // distancia al borde inferior
static constexpr int STATUS_ABOVE = APPLY_MARGIN + APPLY_H + 12; // mensaje justo encima del boton


void ConfigScreen::create(lv_obj_t* parent)
{
    screen = lv_obj_create(parent);
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(screen, 0, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(screen, Theme::Background(), 0);

    createContent();
}

void ConfigScreen::setText(lv_obj_t* label, const char* text)
{
    // Solo toca LVGL si el texto cambio (evita redibujar sin necesidad)
    if (strcmp(lv_label_get_text(label), text) != 0)
        lv_label_set_text(label, text);
}


// ------------------------------------------------------------
// Panel de texto: configuracion actual
// ------------------------------------------------------------

void ConfigScreen::createInfoPanel()
{
    static const char* const LABELS[INFO_COUNT] =
    {
        "Medición de sensores",
        "Consulta de la tablet",
        "Refresco del tiempo",
        "Refresco del sistema",
        "Reconexión MQTT",
        "Ubicación",
        "Brillo",
        "Apagado de pantalla (tablet)"
    };

    lv_obj_t* panel = lv_obj_create(screen);
    lv_obj_remove_style_all(panel);
    lv_obj_set_size(panel, ContentLayout::screenContentWidth(screen), INFO_COUNT * INFO_ROW_H + PANEL_PAD * 2);
    lv_obj_set_pos(panel, ContentLayout::LEFT, PANEL_TOP);
    lv_obj_set_style_bg_color(panel, Theme::Panel(), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(panel, 16, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < INFO_COUNT; i++)
    {
        const int y = PANEL_PAD + i * INFO_ROW_H + 5;

        lv_obj_t* label = lv_label_create(panel);
        lv_label_set_text(label, LABELS[i]);
        lv_obj_set_style_text_font(label, &montserrat_28_es_16, 0);
        lv_obj_set_style_text_color(label, Theme::TextSecondary(), 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 20, y);

        infoValue[i] = lv_label_create(panel);
        lv_label_set_text(infoValue[i], "--");
        lv_obj_set_style_text_font(infoValue[i], &montserrat_28_es_16, 0);
        lv_obj_set_style_text_color(infoValue[i], Theme::Text(), 0);
        lv_obj_align(infoValue[i], LV_ALIGN_TOP_RIGHT, -20, y);
    }
}


// ------------------------------------------------------------
// Botones -/+ de cada intervalo (solo cambian el borrador)
// ------------------------------------------------------------

void ConfigScreen::createStepper(uint8_t row, int y, const char* title, const char* hint)
{
    lv_obj_t* label = lv_label_create(screen);
    lv_label_set_text(label, title);
    lv_obj_set_style_text_font(label, &montserrat_28_es_20, 0);
    lv_obj_set_style_text_color(label, Theme::Text(), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, y);

    lv_obj_t* hintLabel = lv_label_create(screen);
    lv_label_set_text(hintLabel, hint);
    lv_obj_set_style_text_font(hintLabel, &montserrat_28_es_16, 0);
    lv_obj_set_style_text_color(hintLabel, Theme::TextSecondary(), 0);
lv_obj_align(hintLabel, LV_ALIGN_TOP_MID, 0, y + 24);

    // Boton menos
    lv_obj_t* minusBtn = lv_button_create(screen);
lv_obj_set_size(minusBtn, 70, 48);
    lv_obj_set_style_radius(minusBtn, 16, 0);
lv_obj_align(minusBtn, LV_ALIGN_TOP_MID, -140, y + 44);
    lv_obj_t* minusLabel = lv_label_create(minusBtn);
    lv_label_set_text(minusLabel, LV_SYMBOL_MINUS);
    lv_obj_center(minusLabel);

    buttonCtx[row * 2] = { this, row, -1 };
    lv_obj_add_event_cb(minusBtn, onStepClicked, LV_EVENT_CLICKED, &buttonCtx[row * 2]);

    // Valor (borrador)
    valueLabel[row] = lv_label_create(screen);
    lv_label_set_text(valueLabel[row], "--");
    lv_obj_set_style_text_font(valueLabel[row], &montserrat_28_es_28, 0);
    lv_obj_set_style_text_color(valueLabel[row], Theme::Text(), 0);
lv_obj_align(valueLabel[row], LV_ALIGN_TOP_MID, 0, y + 51);

    // Boton mas
    lv_obj_t* plusBtn = lv_button_create(screen);
lv_obj_set_size(plusBtn, 70, 48);
    lv_obj_set_style_radius(plusBtn, 16, 0);
lv_obj_align(plusBtn, LV_ALIGN_TOP_MID, 140, y + 44);
    lv_obj_t* plusLabel = lv_label_create(plusBtn);
    lv_label_set_text(plusLabel, LV_SYMBOL_PLUS);
    lv_obj_center(plusLabel);

    buttonCtx[row * 2 + 1] = { this, row, +1 };
    lv_obj_add_event_cb(plusBtn, onStepClicked, LV_EVENT_CLICKED, &buttonCtx[row * 2 + 1]);
}

// Un selector compacto de hora: [-]  09:00  [+]  (tres de estos caben en una fila)
void ConfigScreen::createHourStepper(uint8_t row, int centerX, int y)
{
    lv_obj_t* minusBtn = lv_button_create(screen);
    lv_obj_set_size(minusBtn, 56, 48);
    lv_obj_set_style_radius(minusBtn, 16, 0);
    lv_obj_align(minusBtn, LV_ALIGN_TOP_MID, centerX - 88, y);

    lv_obj_t* minusLabel = lv_label_create(minusBtn);
    lv_label_set_text(minusLabel, LV_SYMBOL_MINUS);
    lv_obj_center(minusLabel);

    buttonCtx[row * 2] = { this, row, -1 };
    lv_obj_add_event_cb(minusBtn, onStepClicked, LV_EVENT_CLICKED, &buttonCtx[row * 2]);

    valueLabel[row] = lv_label_create(screen);
    lv_label_set_text(valueLabel[row], "--");
    lv_obj_set_width(valueLabel[row], 110);
    lv_obj_set_style_text_align(valueLabel[row], LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(valueLabel[row], &montserrat_28_es_28, 0);
    lv_obj_set_style_text_color(valueLabel[row], Theme::Text(), 0);
    lv_obj_align(valueLabel[row], LV_ALIGN_TOP_MID, centerX, y + 7);

    lv_obj_t* plusBtn = lv_button_create(screen);
    lv_obj_set_size(plusBtn, 56, 48);
    lv_obj_set_style_radius(plusBtn, 16, 0);
    lv_obj_align(plusBtn, LV_ALIGN_TOP_MID, centerX + 88, y);

    lv_obj_t* plusLabel = lv_label_create(plusBtn);
    lv_label_set_text(plusLabel, LV_SYMBOL_PLUS);
    lv_obj_center(plusLabel);

    buttonCtx[row * 2 + 1] = { this, row, +1 };
    lv_obj_add_event_cb(plusBtn, onStepClicked, LV_EVENT_CLICKED, &buttonCtx[row * 2 + 1]);
}


void ConfigScreen::createApplyButton(int y)
{
    applyBtn = lv_button_create(screen);
    lv_obj_set_size(applyBtn, 380, APPLY_H);
    lv_obj_align(applyBtn, LV_ALIGN_BOTTOM_MID, 0, -y);
    lv_obj_set_style_bg_color(applyBtn, Theme::Primary(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(applyBtn, Theme::Panel(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_radius(applyBtn, 16, 0);

    lv_obj_t* applyLabel = lv_label_create(applyBtn);
lv_label_set_text(applyLabel, "Actualizar configuración");
    lv_obj_set_style_text_font(applyLabel, &montserrat_28_es_20, 0);
    lv_obj_center(applyLabel);

    // Sin cambios pendientes no hay nada que aplicar
    lv_obj_add_state(applyBtn, LV_STATE_DISABLED);
    applyEnabled = 0;

    // Se registra el ULTIMO (despues de los 6 botones -/+)
    lv_obj_add_event_cb(applyBtn, onApplyClicked, LV_EVENT_CLICKED, this);
}

void ConfigScreen::createContent()
{
    ContentLayout::createTitle(screen, "Configuración");

    createInfoPanel();

    createStepper(ROW_SENSORS, STEPPER_TOP,
                  "Medición de sensores",
                  "Cada cuánto miden los sensores (aplica en su próximo ciclo)");

    createStepper(ROW_POLL, STEPPER_TOP + STEPPER_PITCH,
                  "Consulta de la tablet",
                  "Cada cuánto esta tablet pide los datos al servidor");

    createStepper(ROW_WEATHER, STEPPER_TOP + STEPPER_PITCH * 2,
                  "Refresco del tiempo",
                  "Cada cuánto el servidor renueva el tiempo");

    createStepper(ROW_SCREEN, STEPPER_TOP + STEPPER_PITCH * 3,
                  "Apagado de pantalla",
                  "Tiempo sin tocar la tablet antes de apagar la pantalla (se guarda en la tablet)");

    // ---- Franjas del pronostico: 3 horas en una sola fila ----
    const int hoursY = STEPPER_TOP + STEPPER_PITCH * 4;

    lv_obj_t* hoursTitle = lv_label_create(screen);
    lv_label_set_text(hoursTitle, "Franjas del tiempo");
    lv_obj_set_style_text_font(hoursTitle, &montserrat_28_es_20, 0);
    lv_obj_set_style_text_color(hoursTitle, Theme::Text(), 0);
    lv_obj_align(hoursTitle, LV_ALIGN_TOP_MID, 0, hoursY);

    lv_obj_t* hoursHint = lv_label_create(screen);
    lv_label_set_text(hoursHint, "Hora de cada franja del pronóstico (usa la medición más cercana)");
    lv_obj_set_style_text_font(hoursHint, &montserrat_28_es_16, 0);
    lv_obj_set_style_text_color(hoursHint, Theme::TextSecondary(), 0);
    lv_obj_align(hoursHint, LV_ALIGN_TOP_MID, 0, hoursY + 24);

    createHourStepper(ROW_HOUR1, -250, hoursY + 44);
    createHourStepper(ROW_HOUR2,    0, hoursY + 44);
    createHourStepper(ROW_HOUR3,  250, hoursY + 44);

    // Para que el primer repintado ponga el texto aunque el valor sea 0
    for (uint8_t i = 0; i < ROW_COUNT; i++)
        shownValue[i] = 0xFFFFFFFFu;

    createApplyButton(APPLY_MARGIN);

    // Estado (cambios sin aplicar / Guardando / Guardado / Error)
    statusLabel = lv_label_create(screen);
    lv_label_set_text(statusLabel, "");
    lv_obj_set_style_text_font(statusLabel, &montserrat_28_es_20, 0);
    lv_obj_set_style_text_color(statusLabel, Theme::TextSecondary(), 0);
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -STATUS_ABOVE);
}


// ============================================================
// Interaccion
// ============================================================

void ConfigScreen::onStepClicked(lv_event_t* e)
{
    ButtonCtx* ctx = static_cast<ButtonCtx*>(lv_event_get_user_data(e));

    if (ctx && ctx->self)
        ctx->self->step(ctx->row, ctx->dir);
}

void ConfigScreen::onApplyClicked(lv_event_t* e)
{
    ConfigScreen* self = static_cast<ConfigScreen*>(lv_event_get_user_data(e));

    if (self)
        self->applyDrafts();
}

void ConfigScreen::step(uint8_t row, int dir)
{
    // Solo toca el borrador: no hay red, ni NVS, ni mutex.
    // (Se lee el valor aplicado sin mutex: es un uint32 y solo se usa para
    // decidir si la fila queda "modificada".)
    ApiService& api = ServiceManager::api();

    uint32_t applied = 0;

    switch (row)
    {
        case ROW_SENSORS:
            applied = api.settings().apiRefreshSeconds;
            draft[row] = nextPreset(SENSOR_PRESETS, touched[row] ? draft[row] : applied, dir);
            break;

        case ROW_POLL:
            applied = api.pollIntervalSeconds();
            draft[row] = nextPreset(POLL_PRESETS, touched[row] ? draft[row] : applied, dir);
            break;

        case ROW_WEATHER:
            applied = api.settings().weatherRefreshSeconds;
            draft[row] = nextPreset(WEATHER_PRESETS, touched[row] ? draft[row] : applied, dir);
            break;

        case ROW_SCREEN:
            applied = screenUiValue(ServiceManager::power().inactivityTimeoutSeconds());
            draft[row] = nextPreset(SCREEN_PRESETS, touched[row] ? draft[row] : applied, dir);
            break;

        case ROW_HOUR1:
        case ROW_HOUR2:
        case ROW_HOUR3:
        {
            applied = api.settings().forecastHour[row - ROW_HOUR1];
            const uint32_t current = touched[row] ? draft[row] : applied;
            draft[row] = (uint32_t)((int)current + 24 + dir) % 24;   // 0-23, da la vuelta
            break;
        }

        default:
            return;
    }

    // Si el usuario vuelve al valor original, la fila deja de estar modificada
    touched[row] = (draft[row] != applied);

    refresh();
}

void ConfigScreen::applyDrafts()
{
    ApiService& api = ServiceManager::api();

    // Tablet -> servidor: local (NVS), sin red
    if (touched[ROW_POLL])
        api.setPollIntervalSeconds(draft[ROW_POLL]);

    // Apagado de pantalla: local (NVS), sin red
    if (touched[ROW_SCREEN])
        ServiceManager::power().setInactivityTimeoutSeconds(screenStoredValue(draft[ROW_SCREEN]));

    // Servidor: un unico POST con SOLO los valores modificados
    api.requestBackendSettings(
        touched[ROW_SENSORS] ? draft[ROW_SENSORS] : 0,
        touched[ROW_WEATHER] ? draft[ROW_WEATHER] : 0);

    // Franjas del pronostico: mismo POST al servidor, solo las modificadas
    {
        int8_t hours[3];
        bool anyHour = false;

        for (uint8_t k = 0; k < 3; k++)
        {
            const uint8_t row = ROW_HOUR1 + k;
            hours[k] = touched[row] ? (int8_t)draft[row] : (int8_t)-1;
            anyHour = anyHour || touched[row];
        }

        if (anyHour)
            api.requestForecastHours(hours[0], hours[1], hours[2]);
    }

    for (uint8_t i = 0; i < ROW_COUNT; i++)
        touched[i] = false;

    refresh();
}


// ============================================================
// Repintado
// ============================================================

void ConfigScreen::onRefreshTimer(lv_timer_t* timer)
{
    ConfigScreen* self =
        static_cast<ConfigScreen*>(lv_timer_get_user_data(timer));

    if (self)
        self->refresh();
}

void ConfigScreen::refresh()
{
    uint32_t values[ROW_COUNT];
    uint32_t systemRefresh, mqttReconnect;
    unsigned brightness;
    bool configured;
    char location[48];
    ApiService::SaveState state;

    ServiceManager::lockData();

    ApiService& api = ServiceManager::api();
    const SettingsInfo& s = api.settings();

    values[ROW_SENSORS] = s.apiRefreshSeconds;
    values[ROW_POLL]    = api.pollIntervalSeconds();
    values[ROW_WEATHER] = s.weatherRefreshSeconds;
    values[ROW_SCREEN]  = screenUiValue(ServiceManager::power().inactivityTimeoutSeconds());
    values[ROW_HOUR1]   = s.forecastHour[0];
    values[ROW_HOUR2]   = s.forecastHour[1];
    values[ROW_HOUR3]   = s.forecastHour[2];

    configured    = s.configured;
    systemRefresh = s.systemRefreshSeconds;
    mqttReconnect = s.mqttReconnectSeconds;
    brightness    = (unsigned)s.brightness;
    snprintf(location, sizeof(location), "%s", s.deviceLocation.c_str());

    state = api.saveState();

    ServiceManager::unlockData();

    char buf[48];

    // ---- borradores: la fila sin tocar sigue al valor aplicado ----
    bool anyTouched = false;

    for (uint8_t i = 0; i < ROW_COUNT; i++)
    {
        if (touched[i] && draft[i] == values[i])
            touched[i] = false;     // el servidor ya tiene ese valor

        if (!touched[i])
            draft[i] = values[i];

        anyTouched = anyTouched || touched[i];

        if (draft[i] != shownValue[i])
        {
            shownValue[i] = draft[i];

            if (i >= ROW_HOUR1)
                snprintf(buf, sizeof(buf), "%02u:00", (unsigned)draft[i]);
            else
                formatSeconds(draft[i], buf, sizeof(buf));
            lv_label_set_text(valueLabel[i], buf);
        }

        // Una fila modificada se resalta hasta que se aplique
        if (touched[i] != shownTouched[i])
        {
            shownTouched[i] = touched[i];

            lv_obj_set_style_text_color(
                valueLabel[i],
                touched[i] ? Theme::Primary() : Theme::Text(),
                0);
        }
    }

    // ---- boton "Actualizar intervalos" ----
    const int8_t wantEnabled = anyTouched ? 1 : 0;

    if (wantEnabled != applyEnabled)
    {
        applyEnabled = wantEnabled;

        if (anyTouched)
            lv_obj_remove_state(applyBtn, LV_STATE_DISABLED);
        else
            lv_obj_add_state(applyBtn, LV_STATE_DISABLED);
    }

    // ---- texto: configuracion actual (valores APLICADOS) ----
    // Lo local (consulta y apagado de pantalla) siempre esta disponible; el resto
    // viene del servidor. Los valores se muestran SIEMPRE (antes se tapaban con
    // "--" hasta recibir la configuracion y, si no llegaba, no se veia nada).
    formatSeconds(values[ROW_POLL], buf, sizeof(buf));
    setText(infoValue[INFO_POLL], buf);

    formatSeconds(values[ROW_SCREEN], buf, sizeof(buf));
    setText(infoValue[INFO_SCREEN_TIMEOUT], buf);

    formatSeconds(values[ROW_SENSORS], buf, sizeof(buf));
    setText(infoValue[INFO_SENSORS], buf);

    formatSeconds(values[ROW_WEATHER], buf, sizeof(buf));
    setText(infoValue[INFO_WEATHER], buf);

    formatSeconds(systemRefresh, buf, sizeof(buf));
    setText(infoValue[INFO_SYSTEM], buf);

    formatSeconds(mqttReconnect, buf, sizeof(buf));
    setText(infoValue[INFO_MQTT], buf);

    setText(infoValue[INFO_LOCATION], location[0] ? location : "--");

    snprintf(buf, sizeof(buf), "%u", brightness);
    setText(infoValue[INFO_BRIGHTNESS], buf);

    // ---- estado ----
    const uint8_t st = static_cast<uint8_t>(state);

    if (st != lastState)
    {
        lastState = st;
        stateSince = millis();
        showResult = true;      // hubo un cambio de estado en esta visita
    }

    const char* text = "";
    lv_color_t color = Theme::TextSecondary();

    if (anyTouched)
    {
        text = "Hay cambios sin aplicar";
    }
    else if (!configured && state == ApiService::SaveState::Idle)
    {
        text = "Esperando la configuración del servidor...";
    }
    else
    {
        switch (state)
        {
            case ApiService::SaveState::Saving:
                text = "Guardando...";
                break;

            case ApiService::SaveState::Saved:
                if (showResult && millis() - stateSince <= 3000)   // "Guardado" se oculta a los 3 s
                {
                    text = "Guardado";
                    color = Theme::Success();
                }
                break;

            case ApiService::SaveState::Failed:
                if (showResult)
                {
                    text = "El servidor no guardó el cambio";
                    color = Theme::Error();
                }
                break;

            default:
                break;
        }
    }

    if (strcmp(lv_label_get_text(statusLabel), text) != 0)
    {
        lv_label_set_text(statusLabel, text);
        lv_obj_set_style_text_color(statusLabel, color, 0);
    }
}

void ConfigScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);

    // Empieza sin borradores y se repinta todo al entrar
    for (uint8_t i = 0; i < ROW_COUNT; i++)
    {
        touched[i] = false;
        shownValue[i] = 0;
    }

    // Un resultado de una visita anterior no se vuelve a mostrar
    lastState = static_cast<uint8_t>(ServiceManager::api().saveState());
    showResult = false;

    if (!refreshTimer)
        refreshTimer = lv_timer_create(onRefreshTimer, 200, this);

    // show() puede llamarse con el mutex de datos tomado, asi que no
    // se repinta aqui: el timer lo hace en el siguiente ciclo de LVGL.
    lv_timer_ready(refreshTimer);
}

void ConfigScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);

    // Salir de la pantalla descarta los cambios sin aplicar
    for (uint8_t i = 0; i < ROW_COUNT; i++)
        touched[i] = false;

    if (refreshTimer)
    {
        lv_timer_delete(refreshTimer);
        refreshTimer = nullptr;
    }
}

void ConfigScreen::destroy()
{
    if (refreshTimer)
    {
        lv_timer_delete(refreshTimer);
        refreshTimer = nullptr;
    }

    if (screen)
    {
        lv_obj_delete(screen);

        screen = nullptr;
    }
}
