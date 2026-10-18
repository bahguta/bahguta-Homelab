#include "ConfigCard.h"

#include "../../../fonts/Fonts.h"
#include "../../themes/Theme.h"
#include <Arduino.h>

ConfigCard::ConfigCard(lv_obj_t* parent)
    : Card(parent)
{
    setClickScreen(ScreenType::Config);

    lv_obj_set_style_text_color(
        root,
        Theme::Text(),
        0
    );
}

void ConfigCard::layout()
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

    lv_obj_set_style_radius(root, 16, 0);

    // -------------------------
    // Icon (símbolo LVGL, sin bitmap propio)
    // -------------------------

    iconLabel = lv_label_create(root);
    lv_label_set_text(iconLabel, LV_SYMBOL_POWER);
    lv_obj_set_style_text_font(iconLabel, &montserrat_28_es_26, 0);
    lv_obj_align(iconLabel, LV_ALIGN_TOP_LEFT, 0, 0);

    // -------------------------
    // Title
    // -------------------------

    titleLabel = lv_label_create(root);

    lv_label_set_text(
        titleLabel,
        "Config"
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
    // Status
    // -------------------------

    statusLabel = lv_label_create(root);

    lv_label_set_text(
        statusLabel,
        "Apagado / Reinicio"
    );

    lv_obj_set_style_text_color(
        statusLabel,
        Theme::Text(),
        0
    );

    lv_obj_set_style_text_font(
        statusLabel,
        &montserrat_28_es_14,
        0
    );

    lv_obj_align(
        statusLabel,
        LV_ALIGN_BOTTOM_LEFT,
        0,
        0
    );
}