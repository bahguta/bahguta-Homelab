#include "RedCard.h"

#include "../../../fonts/Fonts.h"
#include "../../themes/Theme.h"
#include <Arduino.h>

RedCard::RedCard(lv_obj_t* parent)
    : Card(parent)
{
    setClickScreen(ScreenType::Red);

    lv_obj_set_style_text_color(root, Theme::Text(), 0);
}

void RedCard::layout()
{
    lv_obj_set_style_pad_all(root, PADDING, 0);
    lv_obj_set_style_radius(root, 16, 0);

    // Icono (simbolo LVGL con la fuente por defecto, que si trae los simbolos)
    iconLabel = lv_label_create(root);
    lv_label_set_text(iconLabel, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(iconLabel, Theme::Text(), 0);
    lv_obj_align(iconLabel, LV_ALIGN_TOP_LEFT, 0, 6);

    titleLabel = lv_label_create(root);
    lv_label_set_text(titleLabel, "Red");
    lv_obj_set_style_text_color(titleLabel, Theme::Text(), 0);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_LEFT, 42, 6);

    // Numero grande de equipos conectados
    countLabel = lv_label_create(root);
    lv_label_set_text(countLabel, "--");
    lv_obj_set_style_text_font(countLabel, &montserrat_28_es_40, 0);
    lv_obj_set_style_text_color(countLabel, Theme::Text(), 0);
    lv_obj_align(countLabel, LV_ALIGN_CENTER, 0, 4);

    subLabel = lv_label_create(root);
    lv_label_set_text(subLabel, "conectados");
    lv_obj_set_style_text_font(subLabel, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(subLabel, Theme::TextSecondary(), 0);
    lv_obj_align(subLabel, LV_ALIGN_BOTTOM_LEFT, 0, 0);
}

void RedCard::setClients(int total, bool available)
{
    if (!countLabel)
        return;

    char buffer[16];

    if (available)
        snprintf(buffer, sizeof(buffer), "%d", total);
    else
        snprintf(buffer, sizeof(buffer), "--");

    if (strcmp(lv_label_get_text(countLabel), buffer) != 0)
    {
        lv_label_set_text(countLabel, buffer);
        lv_obj_align(countLabel, LV_ALIGN_CENTER, 0, 4);
    }
}
