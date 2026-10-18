#include "SettingsCard.h"
#include "../../../icons/Icons.h"

#include "../../../fonts/Fonts.h"
#include "../../themes/Theme.h"
#include <Arduino.h>

SettingsCard::SettingsCard(lv_obj_t* parent)
    : Card(parent)
{
    setClickScreen(ScreenType::Settings);

    lv_obj_set_style_text_color(
        root,
        Theme::Text(),
        0
    );
}

void SettingsCard::layout()
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
    lv_image_set_src(iconChip, &icon_settings);
    lv_obj_align(iconChip, LV_ALIGN_TOP_LEFT, 0, 0);

    titleLabel = lv_label_create(root);

    lv_label_set_text(
        titleLabel,
        "Settings"
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
        "Configuración"
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

void SettingsCard::setStatus(const char* text)
{
    if (!statusLabel)
        return;

    lv_label_set_text(
        statusLabel,
        text
    );
}