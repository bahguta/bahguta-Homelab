#include "WeatherScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../services/ServiceManager.h"
#include "../../models/HomeInfo.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------
// Geometria. La card del tiempo ocupa lo mismo que en el Home
// (ContentLayout::WEATHER_HEIGHT) y debajo van 5 filas que se reparten TODO
// el alto que queda hasta el final de la pantalla (se calcula, no se supone).
// ---------------------------------------------------------------------
static constexpr int BODY_WIDTH_DEFAULT = 800;
static constexpr int BODY_HEIGHT_DEFAULT = 970;
static constexpr int ROWS_GAP = 8;
static constexpr int ROW_H_MIN = 80;
static constexpr int ROW_H_MAX = 120;
static constexpr int DAY_COL_W = 140;     // dia y fecha
static constexpr int ROW_RIGHT_PAD = 10;
static constexpr int SLOT_PAD = 10;       // margen dentro de cada franja
static constexpr int TEXT_W = 106;        // hora y min / max (a la izquierda del icono)
static constexpr int SLOT_ICON = 88;      // icono de cada franja (data/weather-slot, PNG de 88 px)
static constexpr int SLOT_ICON_PAD = 4;   // margen derecho del icono dentro de la franja


void WeatherScreen::create(lv_obj_t* parent)
{
    screen = lv_obj_create(parent);
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(screen, 0, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(screen, Theme::Background(), 0);

    // Igual que HomeScreen: hay que resolver el tamano de la pantalla ANTES de
    // medirla. El body tiene padding, asi que el ancho real es menor que 800; sin
    // medirlo la card salia mas ancha que la del Home.
    lv_obj_update_layout(screen);

    createContent();
}


int WeatherScreen::bodyWidthPx() const
{
    const int w = lv_obj_get_width(screen);

    return (w > 0) ? w : BODY_WIDTH_DEFAULT;
}

int WeatherScreen::bodyHeightPx() const
{
    const int h = lv_obj_get_height(screen);

    return (h > 400) ? h : BODY_HEIGHT_DEFAULT;
}


void WeatherScreen::createContent()
{
    // La MISMA posicion y tamano que la card del Home (mismas llamadas y mismo
    // orden que HomeScreen::layoutContent): al cambiar de pantalla no se nota
    // ninguna diferencia
    const int bodyWidth = bodyWidthPx();

    weatherCard = new WeatherCard(screen);

    lv_obj_set_pos(weatherCard->object(), ContentLayout::LEFT, ContentLayout::TOP);

    lv_obj_set_size(
        weatherCard->object(),
        ContentLayout::contentWidth(bodyWidth),
        ContentLayout::WEATHER_HEIGHT
    );

    weatherCard->layout();

    createForecastRows();
}


static const char* forecastIconFolder(const char* icon)
{
    if (!icon || strlen(icon) < 3)
        return "not-available";

    const bool night = (icon[2] == 'n');

    if (strncmp(icon, "01", 2) == 0)
        return night ? "clear-night" : "clear-day";

    if (strncmp(icon, "02", 2) == 0)
        return night ? "partly-cloudy-night" : "overcast";

    if (strncmp(icon, "03", 2) == 0 ||
        strncmp(icon, "04", 2) == 0)
        return night ? "cloudy-night" : "cloudy";

    if (strncmp(icon, "09", 2) == 0 ||
        strncmp(icon, "10", 2) == 0)
        return night ? "rain-night" : "rain";

    if (strncmp(icon, "11", 2) == 0)
        return night ? "thunderstorms-night" : "thunderstorms-day-rain";

    if (strncmp(icon, "13", 2) == 0)
        return night ? "snow-night" : "snow";

    if (strncmp(icon, "50", 2) == 0)
        return night ? "cloudy-night" : "overcast";

    return "not-available";
}


/*
 * Icono de una franja: la misma escena que usa la card del tiempo, a 56 px.
 * Se usa data/weather-slot/<escena>.png; si esa carpeta aun no esta subida a
 * LittleFS se escala el frame completo (128 px * 176/256 = 88 px) para que no
 * quede vacio.
 */
static void setSlotIcon(lv_obj_t* img, const char* icon)
{
    const char* folder = forecastIconFolder(icon);

    char slot[56];
    snprintf(slot, sizeof(slot), "/weather-slot/%s.png", folder);

    char path[72];

    if (LittleFS.exists(slot))
    {
        lv_image_set_scale(img, 256);
        snprintf(path, sizeof(path), "L:%s", slot);
    }
    else
    {
        lv_image_set_pivot(img, 0, 0);      // escalar desde la esquina, no desde el centro
        lv_image_set_scale(img, 176);   // 128 px * 176/256 = 88 px
        snprintf(path, sizeof(path), "L:/weather/%s/frame_001.png", folder);
    }

    lv_image_set_src(img, path);
}


static lv_obj_t* createDivider(lv_obj_t* row, int x, int rowH)
{
    lv_obj_t* divider = lv_obj_create(row);
    lv_obj_remove_style_all(divider);
    lv_obj_set_size(divider, 1, rowH - 30);
    lv_obj_set_pos(divider, x, 15);
    lv_obj_set_style_bg_color(divider, Theme::TextSecondary(), 0);
    lv_obj_set_style_bg_opa(divider, LV_OPA_30, 0);
    return divider;
}

void WeatherScreen::createForecastRows()
{
    const int width = ContentLayout::contentWidth(bodyWidthPx());
    const int top = ContentLayout::TOP + ContentLayout::WEATHER_HEIGHT + ContentLayout::GAP;

    // Las 5 filas se reparten TODO el alto que queda hasta el final de la pantalla
    int rowH = (bodyHeightPx() - top - (FORECAST_ROWS - 1) * ROWS_GAP) / FORECAST_ROWS;

    if (rowH < ROW_H_MIN) rowH = ROW_H_MIN;
    if (rowH > ROW_H_MAX) rowH = ROW_H_MAX;

    rowHeight = rowH;

    // Cada franja: texto (hora y min / max) a la izquierda e icono a la derecha
    const int slotW = (width - DAY_COL_W - ROW_RIGHT_PAD) / WeatherInfo::FORECAST_HOURS;
    const int textY = (rowH - 48) / 2;

    for (int i = 0; i < FORECAST_ROWS; i++)
    {
        lv_obj_t* row = lv_obj_create(screen);
        lv_obj_remove_style_all(row);
        lv_obj_set_pos(row, ContentLayout::LEFT, top + i * (rowH + ROWS_GAP));
        lv_obj_set_size(row, width, rowH);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_color(row, Theme::Panel(), 0);
        lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(row, 12, 0);

        // ---- izquierda: dia y fecha ----
        lv_obj_t* dayLabel = lv_label_create(row);
        lv_label_set_text(dayLabel, "--\n--/--");
        lv_obj_set_style_text_font(dayLabel, &montserrat_28_es_20, 0);
        lv_obj_set_style_text_color(dayLabel, Theme::Text(), 0);
        lv_obj_set_style_text_align(dayLabel, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(dayLabel, LV_ALIGN_LEFT_MID, 16, 0);

        // separador entre el dia y la primera franja
        createDivider(row, DAY_COL_W - 6, rowH);

        // ---- tres franjas: [hora / min-max] [icono] | ----
        for (int k = 0; k < WeatherInfo::FORECAST_HOURS; k++)
        {
            const int x0 = DAY_COL_W + k * slotW;

            if (k > 0)
                createDivider(row, x0, rowH);

            lv_obj_t* hour = lv_label_create(row);
            lv_label_set_text(hour, "");
            lv_obj_set_style_text_font(hour, &montserrat_28_es_16, 0);
            lv_obj_set_style_text_color(hour, Theme::TextSecondary(), 0);
            lv_obj_set_style_text_align(hour, LV_TEXT_ALIGN_LEFT, 0);
            lv_obj_set_size(hour, TEXT_W, 20);
            lv_obj_set_pos(hour, x0 + SLOT_PAD, textY);

            lv_obj_t* temp = lv_label_create(row);
            lv_label_set_text(temp, "");
            lv_obj_set_style_text_font(temp, &montserrat_28_es_20, 0);
            lv_obj_set_style_text_color(temp, Theme::Text(), 0);
            lv_obj_set_style_text_align(temp, LV_TEXT_ALIGN_LEFT, 0);
            lv_obj_set_size(temp, TEXT_W, 26);
            lv_obj_set_pos(temp, x0 + SLOT_PAD, textY + 22);

            lv_obj_t* icon = lv_image_create(row);
            lv_obj_set_size(icon, SLOT_ICON, SLOT_ICON);
            lv_obj_set_pos(icon, x0 + slotW - SLOT_ICON_PAD - SLOT_ICON, (rowH - SLOT_ICON) / 2);

            lv_obj_add_flag(hour, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(temp, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(icon, LV_OBJ_FLAG_HIDDEN);

            hourLabel[i][k] = hour;
            hourTemp[i][k] = temp;
            hourIcon[i][k] = icon;
        }

        // texto cuando un dia no trae franjas (p. ej. hoy a ultima hora)
        lv_obj_t* empty = lv_label_create(row);
        lv_label_set_text(empty, "Sin franjas disponibles");
        lv_obj_set_style_text_font(empty, &montserrat_28_es_16, 0);
        lv_obj_set_style_text_color(empty, Theme::TextSecondary(), 0);
        lv_obj_set_style_text_align(empty, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_size(empty, slotW * WeatherInfo::FORECAST_HOURS, 20);
        lv_obj_set_pos(empty, DAY_COL_W, (rowH - 20) / 2);
        lv_obj_add_flag(empty, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(row, LV_OBJ_FLAG_HIDDEN);

        forecastRow[i] = row;
        forecastDayLabel[i] = dayLabel;
        forecastEmpty[i] = empty;
    }
}


void WeatherScreen::update()
{
    const HomeInfo& home =
        ServiceManager::home().home();

    if (!home.weather.success)
        return;

    weatherCard->setLocation(
        home.weather.location.c_str()
    );

    char temperature[16];

    snprintf(
        temperature,
        sizeof(temperature),
        "%.1f °C",
        home.weather.temperature
    );

    weatherCard->setTemperature(temperature);

    weatherCard->setDescription(
        home.weather.description.c_str()
    );

    weatherCard->setIcon(
        home.weather.icon.c_str()
    );

    weatherCard->setHumidity(home.weather.humidity);
    weatherCard->setWind(home.weather.windSpeed);
    weatherCard->setPressure(home.weather.pressure);

    weatherCard->setSunrise(home.weather.sunrise);
    weatherCard->setSunset(home.weather.sunset);

    // ---- pronostico: una fila por dia ----
    for (int i = 0; i < FORECAST_ROWS; i++)
    {
        if (i >= home.weather.forecastCount)
        {
            lv_obj_add_flag(forecastRow[i], LV_OBJ_FLAG_HIDDEN);
            continue;
        }

        lv_obj_clear_flag(forecastRow[i], LV_OBJ_FLAG_HIDDEN);

        const WeatherInfo::ForecastEntry& entry =
            home.weather.forecast[i];

        lv_label_set_text(forecastDayLabel[i], entry.label);

        for (int k = 0; k < WeatherInfo::FORECAST_HOURS; k++)
        {
            if (k >= entry.hourCount)
            {
                lv_obj_add_flag(hourLabel[i][k], LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(hourTemp[i][k], LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(hourIcon[i][k], LV_OBJ_FLAG_HIDDEN);
                continue;
            }

            const WeatherInfo::ForecastHour& slot = entry.hours[k];

            char text[24];

            snprintf(text, sizeof(text), "%02u:00", (unsigned)slot.hour);
            lv_label_set_text(hourLabel[i][k], text);

            // "13° / 18°", o una sola cifra si minima y maxima coinciden al redondear
            const int mn = (int)lroundf(slot.minTemperature);
            const int mx = (int)lroundf(slot.maxTemperature);

            if (mn == mx)
                snprintf(text, sizeof(text), "%d°", mx);
            else
                snprintf(text, sizeof(text), "%d° / %d°", mn, mx);

            lv_label_set_text(hourTemp[i][k], text);

            setSlotIcon(hourIcon[i][k], slot.icon);

            lv_obj_clear_flag(hourLabel[i][k], LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(hourTemp[i][k], LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(hourIcon[i][k], LV_OBJ_FLAG_HIDDEN);
        }

        if (entry.hourCount == 0)
            lv_obj_clear_flag(forecastEmpty[i], LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_add_flag(forecastEmpty[i], LV_OBJ_FLAG_HIDDEN);
    }
}


void WeatherScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);

    weatherCard->resume();

    update();
}


void WeatherScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);

    weatherCard->pause();
}


void WeatherScreen::destroy()
{
    if (screen)
    {
        lv_obj_delete(screen);

        screen = nullptr;
    }
}
