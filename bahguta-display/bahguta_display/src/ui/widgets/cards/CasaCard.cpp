#include "CasaCard.h"
#include <string.h>
#include "../../../icons/Icons.h"

#include "../../themes/Theme.h"
#include <Arduino.h>

CasaCard::CasaCard(lv_obj_t* parent)
    : Card(parent)
{

    setClickScreen(ScreenType::Casa);

    lv_obj_set_style_text_color(
        root,
        Theme::Text(),
        0
    );
}

void CasaCard::layout()
{
    lv_obj_set_style_text_color(
        root,
        Theme::Text(),
        0
    );

    lv_obj_set_style_pad_all(
        root,
        PADDING,
        0
    );

    lv_obj_set_style_radius(
        root,
        0,
        0
    );

    // -------------------------
    // Title
    // -------------------------

    lv_obj_set_style_radius(root, 16, 0);

    lv_obj_t* iconChip = lv_image_create(root);
    lv_image_set_src(iconChip, &icon_casa);
    lv_obj_align(iconChip, LV_ALIGN_TOP_LEFT, 0, 0);

    titleLabel = lv_label_create(root);

    lv_label_set_text(
        titleLabel,
        "Casa"
    );

    lv_obj_set_style_text_color(
        titleLabel,
        Theme::Text(),
        0
    );

    lv_obj_align(
        titleLabel,
        LV_ALIGN_TOP_LEFT,
        42,
        6
    );


    // -------------------------
    // Temperature
    // -------------------------

    temperatureLabel = lv_label_create(root);

    lv_label_set_text(
        temperatureLabel,
        "--.- °C"
    );

    lv_obj_set_style_text_color(
        temperatureLabel,
        Theme::Text(),
        0
    );

    lv_obj_align(
        temperatureLabel,
        LV_ALIGN_CENTER,
        0,
        0
    );


    // -------------------------
    // Rooms
    // -------------------------

    roomsLabel = lv_label_create(root);

    lv_label_set_text(
        roomsLabel,
        "0 habitaciones"
    );

    lv_obj_set_style_text_color(
        roomsLabel,
        Theme::Text(),
        0
    );

    lv_obj_align(
        roomsLabel,
        LV_ALIGN_BOTTOM_LEFT,
        0,
        0
    );
}

void CasaCard::setTemperature(double temperature)
{
    setTemperature(temperature, true);
}


void CasaCard::setTemperature(double temperature, bool available)
{
    if (!temperatureLabel)
        return;

    char buffer[32];

    if (available)
        snprintf(buffer, sizeof(buffer), "%.1f °C", temperature);
    else
        snprintf(buffer, sizeof(buffer), "--");

    if (strcmp(lv_label_get_text(temperatureLabel), buffer) != 0)
        lv_label_set_text(temperatureLabel, buffer);
}

void CasaCard::setClimate(double temperature, bool temperatureAvailable,
                          double humidity, bool humidityAvailable)
{
    if (!temperatureLabel)
        return;

    char t[16];
    char h[16];

    if (temperatureAvailable)
        snprintf(t, sizeof(t), "%.1f °C", temperature);
    else
        snprintf(t, sizeof(t), "--");

    if (humidityAvailable)
        snprintf(h, sizeof(h), "%.0f %%", humidity);
    else
        snprintf(h, sizeof(h), "-- %%");

    char buffer[40];
    snprintf(buffer, sizeof(buffer), "%s   %s", t, h);

    if (strcmp(lv_label_get_text(temperatureLabel), buffer) != 0)
        lv_label_set_text(temperatureLabel, buffer);
}


void CasaCard::setRooms(int rooms)
{
    if (!roomsLabel)
        return;

    char buffer[32];

    snprintf(
        buffer,
        sizeof(buffer),
        "%d habitaciones",
        rooms
    );

    lv_label_set_text(
        roomsLabel,
        buffer
    );
}