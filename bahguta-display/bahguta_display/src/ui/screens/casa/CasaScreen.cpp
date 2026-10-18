#include "CasaScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../services/ServiceManager.h"

#include <Arduino.h>
#include <string.h>

/*
 * IMPORTANTE (backend): el match de cada habitacion se hace
 * buscando estos textos (sin acentos, en minuscula) DENTRO de
 * "id" o "name" de cada RoomInfo. Si tu backend usa slugs de
 * id distintos (ej. "study" en vez de "despacho"), ajusta el
 * campo matchInclude/matchExclude de cada RoomBox en
 * CasaScreen.h -- es lo unico que haria falta tocar.
 */

static const char* ROOM_LABELS[CasaScreen::ROOM_COUNT] = {
    "Despacho",
    "Dormitorio\ninfantil",
    "Dormitorio",
    "Baño",
    "Salon",
    "Recibidor",
    "Baño\ninfantil",
    "Pasillo",
    "Cocina",
};

const char* CasaScreen::roomLabel(int index)
{
    return ROOM_LABELS[index];
}


static bool containsIgnoreCase(const String& haystack, const char* needle)
{
    if (haystack.length() == 0)
        return false;

    String h = haystack;
    h.toLowerCase();

    return h.indexOf(needle) >= 0;
}


static RoomInfo* findRoom(
    CasaInfo& casa,
    const char* include,
    const char* exclude
)
{
    for (int i = 0; i < casa.roomCount; i++)
    {
        bool hasInclude =
            containsIgnoreCase(casa.rooms[i].id, include) ||
            containsIgnoreCase(casa.rooms[i].name, include);

        if (!hasInclude)
            continue;

        if (exclude)
        {
            bool hasExclude =
                containsIgnoreCase(casa.rooms[i].id, exclude) ||
                containsIgnoreCase(casa.rooms[i].name, exclude);

            if (hasExclude)
                continue;
        }

        return &casa.rooms[i];
    }

    return nullptr;
}


void CasaScreen::create(lv_obj_t* parent)
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


void CasaScreen::createContent()
{
    // Columna central: titulo, plano, grafica y medias. Mismo ancho (CONTENT_W) y mismo
    // margen (GAP) arriba, entre bloques y abajo; la grafica se estira para llenar el hueco.
    ContentLayout::createTitle(screen, "Casa");

    // El body tiene relleno del tema por defecto, asi que la pantalla mide menos de 800:
    // se usa el ancho real (igual que Home) para que nada se salga y quede recortado.
    contentW = ContentLayout::screenContentWidth(screen);

    lv_obj_t* column = lv_obj_create(screen);
    lv_obj_remove_style_all(column);
    lv_obj_set_size(column, contentW, lv_obj_get_height(screen) - ContentLayout::CONTENT_TOP);
    lv_obj_set_pos(column, ContentLayout::LEFT, ContentLayout::CONTENT_TOP);
    lv_obj_clear_flag(column, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_top(column, 0, 0);
    lv_obj_set_style_pad_bottom(column, GAP, 0);
    lv_obj_set_style_pad_row(column, GAP, 0);
    lv_obj_set_flex_flow(column, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        column,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_START
    );

    createFloorPlan(column);
    createChart(column);
    createInfoPanel(column);
}


// Colores de las cajas del plano (ajustar aqui los hex). Todos los textos van en blanco.
static const uint32_t COLOR_DESPACHO   = 0x2E7D32;   // verde
static const uint32_t COLOR_INFANTIL   = 0xC9A100;   // amarillo (algo oscuro para que el blanco se lea)
static const uint32_t COLOR_DORMITORIO = 0xB3261E;   // rojo
static const uint32_t COLOR_SALON      = 0x4E342E;   // marron oscuro
static const uint32_t COLOR_COCINA     = 0xE65100;   // naranja
static const uint32_t COLOR_BANO       = 0x0F6B6B;   // banos: verde azulado
static const uint32_t COLOR_GRIS       = 0x555A66;   // pasillo y recibidor

static lv_color_t floorPlanColor(const char* include, const char* exclude)
{
    if (strcmp(include, "despacho") == 0)
        return lv_color_hex(COLOR_DESPACHO);

    if (strcmp(include, "infantil") == 0 && exclude && strcmp(exclude, "bano") == 0)
        return lv_color_hex(COLOR_INFANTIL);

    if (strcmp(include, "dormitorio") == 0)
        return lv_color_hex(COLOR_DORMITORIO);

    if (strcmp(include, "salon") == 0)
        return lv_color_hex(COLOR_SALON);

    if (strcmp(include, "cocina") == 0)
        return lv_color_hex(COLOR_COCINA);

    // banos (el del pasillo y el infantil)
    const bool isBath =
        strcmp(include, "bano") == 0 ||
        (strcmp(include, "infantil") == 0 && exclude && strcmp(exclude, "dormitorio") == 0);

    if (isBath)
        return lv_color_hex(COLOR_BANO);

    // pasillo y recibidor
    return lv_color_hex(COLOR_GRIS);
}

void CasaScreen::createFloorPlan(lv_obj_t* parent)
{
    lv_obj_t* plan = lv_obj_create(parent);
    lv_obj_remove_style_all(plan);
    lv_obj_set_size(plan, contentW, PLAN_H);
    lv_obj_clear_flag(plan, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < ROOM_COUNT; i++)
    {
        RoomBox& r = rooms[i];

        r.box = lv_obj_create(plan);
        lv_obj_remove_style_all(r.box);
        // Las coordenadas del plano estan en PLAN_REF_W: se escalan al ancho real
        const int x0 = r.x * contentW / PLAN_REF_W;
        const int x1 = (r.x + r.w) * contentW / PLAN_REF_W;
        lv_obj_set_pos(r.box, x0, r.y);
        lv_obj_set_size(r.box, x1 - x0, r.h);
        lv_obj_clear_flag(r.box, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_set_style_bg_color(
            r.box,
            floorPlanColor(r.matchInclude, r.matchExclude),
            0
        );

        lv_obj_set_style_bg_opa(r.box, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(r.box, 12, 0);
        lv_obj_set_style_border_width(r.box, 1, 0);
        lv_obj_set_style_border_color(r.box, Theme::Background(), 0);

        lv_obj_t* nameLabel = lv_label_create(r.box);
        lv_label_set_text(nameLabel, roomLabel(i));
        lv_obj_set_style_text_align(nameLabel, LV_TEXT_ALIGN_CENTER, 0);
        // Habitaciones estrechas (banos, recibidor): letra mas pequena
        lv_obj_set_style_text_font(
            nameLabel,
            r.w < 100 ? &montserrat_28_es_12 : &montserrat_28_es_16,
            0);

        // El titulo de cada habitacion con sensor lleva el color de su linea en la grafica
        int ordinal = 0;
        for (int k = 0; k < i; k++)
            if (rooms[k].hasSensor)
                ordinal++;

        lv_obj_set_style_text_color(
            nameLabel,
            Theme::Text(),
            0
        );

        if (r.hasSensor)
        {
            lv_obj_align(nameLabel, LV_ALIGN_TOP_MID, 0, 12);

            r.tempLabel = lv_label_create(r.box);
            lv_label_set_text(r.tempLabel, "--°C");
            lv_obj_set_style_text_font(r.tempLabel, &montserrat_28_es_20, 0);
            lv_obj_set_style_text_color(r.tempLabel, Theme::Text(), 0);
            lv_obj_align(r.tempLabel, LV_ALIGN_CENTER, 0, 10);

            // Humedad bajo la temperatura
            r.humLabel = lv_label_create(r.box);
            lv_label_set_text(r.humLabel, "-- %");
            lv_obj_set_style_text_font(r.humLabel, &montserrat_28_es_16, 0);
            lv_obj_set_style_text_color(r.humLabel, Theme::Text(), 0);
            lv_obj_align(r.humLabel, LV_ALIGN_CENTER, 0, 40);
        }
        else
        {
            lv_obj_center(nameLabel);
        }
    }
}


void CasaScreen::createInfoPanel(lv_obj_t* parent)
{
    lv_obj_t* panel = lv_obj_create(parent);
    lv_obj_remove_style_all(panel);
    lv_obj_set_size(panel, contentW, 130);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(panel, Theme::Panel(), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(panel, 16, 0);

    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);

    lv_obj_set_flex_align(
        panel,
        LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );


    // -------------------------
    // TEMPERATURA MEDIA
    // -------------------------

    lv_obj_t* tempGroup = lv_obj_create(panel);
    lv_obj_remove_style_all(tempGroup);
    lv_obj_set_size(tempGroup, 260, 100);
    lv_obj_clear_flag(tempGroup, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* avgTempCaption = lv_label_create(tempGroup);
    lv_label_set_text(avgTempCaption, "TEMPERATURA MEDIA");
    lv_obj_set_style_text_font(avgTempCaption, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(avgTempCaption, Theme::TextSecondary(), 0);
    lv_obj_align(avgTempCaption, LV_ALIGN_TOP_MID, 0, 0);

    avgTempValue = lv_label_create(tempGroup);
    lv_label_set_text(avgTempValue, "--°C");
    lv_obj_set_style_text_font(avgTempValue, &montserrat_28_es_48, 0);
    lv_obj_set_style_text_color(avgTempValue, Theme::Text(), 0);
    lv_obj_align(avgTempValue, LV_ALIGN_BOTTOM_MID, 0, 0);


    // -------------------------
    // SEPARADOR
    // -------------------------

    lv_obj_t* divider = lv_obj_create(panel);
    lv_obj_remove_style_all(divider);
    lv_obj_set_size(divider, 2, 90);
    lv_obj_set_style_bg_color(divider, Theme::Background(), 0);
    lv_obj_set_style_bg_opa(divider, LV_OPA_COVER, 0);


    // -------------------------
    // HUMEDAD MEDIA
    // -------------------------

    lv_obj_t* humidityGroup = lv_obj_create(panel);
    lv_obj_remove_style_all(humidityGroup);
    lv_obj_set_size(humidityGroup, 260, 100);
    lv_obj_clear_flag(humidityGroup, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* avgHumidityCaption = lv_label_create(humidityGroup);
    lv_label_set_text(avgHumidityCaption, "HUMEDAD MEDIA");
    lv_obj_set_style_text_font(avgHumidityCaption, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(avgHumidityCaption, Theme::TextSecondary(), 0);
    lv_obj_align(avgHumidityCaption, LV_ALIGN_TOP_MID, 0, 0);

    avgHumidityValue = lv_label_create(humidityGroup);
    lv_label_set_text(avgHumidityValue, "-- %");
    lv_obj_set_style_text_font(avgHumidityValue, &montserrat_28_es_48, 0);
    lv_obj_set_style_text_color(avgHumidityValue, Theme::Text(), 0);
    lv_obj_align(avgHumidityValue, LV_ALIGN_BOTTOM_MID, 0, 0);
}


void CasaScreen::update()
{
    updateChart();

    CasaInfo& casa = ServiceManager::api().casa();

    if (!casa.success)
        return;

    for (int i = 0; i < ROOM_COUNT; i++)
    {
        RoomBox& r = rooms[i];

        if (!r.hasSensor)
            continue;

        RoomInfo* room =
            findRoom(casa, r.matchInclude, r.matchExclude);

        char buf[16];

        if (room && room->temperatureAvailable)
        {
            snprintf(buf, sizeof(buf), "%.1f°C", room->temperature);
        }
        else
        {
            snprintf(buf, sizeof(buf), "--°C");
        }

        lv_label_set_text(r.tempLabel, buf);

        // Medicion antigua (sensor sin reportar): valor visible pero atenuado
        lv_obj_set_style_text_opa(
            r.tempLabel,
            (room && room->stale) ? LV_OPA_50 : LV_OPA_COVER,
            0
        );

        // Humedad de la habitacion
        char humBuf[16];
        if (room && room->humidityAvailable)
            snprintf(humBuf, sizeof(humBuf), "%.0f %%", room->humidity);
        else
            snprintf(humBuf, sizeof(humBuf), "-- %%");

        lv_label_set_text(r.humLabel, humBuf);
        lv_obj_set_style_text_opa(
            r.humLabel,
            (room && room->stale) ? LV_OPA_50 : LV_OPA_COVER,
            0
        );
    }

    char avgTempBuf[16];

    if (casa.temperatureAvailable)
    {
        snprintf(avgTempBuf, sizeof(avgTempBuf), "%.1f°C", casa.averageTemperature);
    }
    else
    {
        snprintf(avgTempBuf, sizeof(avgTempBuf), "--°C");
    }

    lv_label_set_text(avgTempValue, avgTempBuf);

    /*
     * CasaInfo no trae una humedad media ya calculada -- la
     * promediamos aqui mismo con las habitaciones que la
     * reporten.
     */

    double humiditySum = 0.0;
    int humidityCount = 0;

    for (int i = 0; i < casa.roomCount; i++)
    {
        if (casa.rooms[i].humidityAvailable)
        {
            humiditySum += casa.rooms[i].humidity;
            humidityCount++;
        }
    }

    char avgHumidityBuf[16];

    if (humidityCount > 0)
    {
        snprintf(
            avgHumidityBuf,
            sizeof(avgHumidityBuf),
            "%.0f %%",
            humiditySum / humidityCount
        );
    }
    else
    {
        snprintf(avgHumidityBuf, sizeof(avgHumidityBuf), "-- %%");
    }

    lv_label_set_text(avgHumidityValue, avgHumidityBuf);
}


void CasaScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);

    // Al abrir la pantalla se pide el historico al backend (lo hace la task de runtime)
    lastHistoryRequest = millis();
    ServiceManager::api().requestCasaHistory();

    update();
}


void CasaScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);
}


void CasaScreen::destroy()
{
    if (screen)
    {
        lv_obj_delete(screen);

        screen = nullptr;
    }
}

// ------------------------------------------------------------
// Grafica de historico
// ------------------------------------------------------------

void CasaScreen::createChart(lv_obj_t* parent)
{
    chart.create(parent, contentW, CHART_HEIGHT);
    lv_obj_set_flex_grow(chart.object(), 1);   // ocupa el espacio que sobre
}


/*
 * Colores de las 5 habitaciones con sensor, en el orden del plano
 * (Despacho, Dormitorio infantil, Dormitorio, Salon, Cocina):
 * verde, amarillo, rojo, marron y naranja. Sin azul: la tablet ya es azul.
 * (El marron es un tono claro, "madera", para que se lea sobre el fondo azul.)
 */
lv_color_t CasaScreen::sensorColor(int ordinal)
{
    static const uint32_t PALETTE[5] = {
        0x5BD97A,   // verde
        0xFFE14D,   // amarillo
        0xFF5C5C,   // rojo
        0xC08A5A,   // marron
        0xFF9F2E,   // naranja
    };

    if (ordinal < 0 || ordinal >= 5)
        return Theme::TextSecondary();

    return lv_color_hex(PALETTE[ordinal]);
}


// Posicion (entre las habitaciones con sensor) de la habitacion que lleva ese nombre; -1 si ninguna
int CasaScreen::sensorOrdinalFor(const char* roomName) const
{
    const String name(roomName);

    int ordinal = 0;

    for (int i = 0; i < ROOM_COUNT; i++)
    {
        const RoomBox& r = rooms[i];

        if (!r.hasSensor)
            continue;

        bool match = containsIgnoreCase(name, r.matchInclude);

        if (match && r.matchExclude && containsIgnoreCase(name, r.matchExclude))
            match = false;

        if (match)
            return ordinal;

        ordinal++;
    }

    return -1;
}


void CasaScreen::updateChart()
{
    const uint32_t now = millis();

    // Mientras se mira la pantalla, se refresca cada 5 minutos
    if (now - lastHistoryRequest >= HISTORY_REFRESH_MS)
    {
        lastHistoryRequest = now;
        ServiceManager::api().requestCasaHistory();
    }

    const CasaHistory& history = ServiceManager::api().casaHistory();

    if (history.version == chartVersion)
        return;

    chartVersion = history.version;

    chart.setData(history);

    for (uint8_t i = 0; i < history.seriesCount; i++)
        chart.setSeriesColor(i, sensorColor(sensorOrdinalFor(history.series[i].room)));
}
