#include "GrafanaCard.h"

#include "../../../fonts/Fonts.h"
#include "../../themes/Theme.h"
#include <Arduino.h>

GrafanaCard::GrafanaCard(lv_obj_t* parent)
    : Card(parent)
{
    setClickScreen(ScreenType::Grafana);

    lv_obj_set_style_text_color(root, Theme::Text(), 0);
}

void GrafanaCard::layout()
{
    lv_obj_set_style_pad_all(root, PADDING, 0);
    lv_obj_set_style_radius(root, 16, 0);

    iconLabel = lv_label_create(root);
    lv_label_set_text(iconLabel, LV_SYMBOL_LIST);
    lv_obj_set_style_text_color(iconLabel, Theme::Text(), 0);
    lv_obj_align(iconLabel, LV_ALIGN_TOP_LEFT, 0, 6);

    titleLabel = lv_label_create(root);
    lv_label_set_text(titleLabel, "Grafana");
    lv_obj_set_style_text_color(titleLabel, Theme::Text(), 0);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_LEFT, 42, 6);

    diskLabel = lv_label_create(root);
    lv_label_set_text(diskLabel, "Disco  --");
    lv_obj_set_style_text_font(diskLabel, &montserrat_28_es_16, 0);
    lv_obj_set_style_text_color(diskLabel, Theme::Text(), 0);
    lv_obj_align(diskLabel, LV_ALIGN_LEFT_MID, 0, 18);

    tempLabel = lv_label_create(root);
    lv_label_set_text(tempLabel, "Temp   --");
    lv_obj_set_style_text_font(tempLabel, &montserrat_28_es_16, 0);
    lv_obj_set_style_text_color(tempLabel, Theme::Text(), 0);
    lv_obj_align(tempLabel, LV_ALIGN_BOTTOM_LEFT, 0, 0);
}

void GrafanaCard::setSummary(double diskPercent, double tempC)
{
    if (!diskLabel || !tempLabel)
        return;

    char buffer[32];

    if (diskPercent >= 0)
        snprintf(buffer, sizeof(buffer), "Disco  %.0f %%", diskPercent);
    else
        snprintf(buffer, sizeof(buffer), "Disco  --");

    if (strcmp(lv_label_get_text(diskLabel), buffer) != 0)
        lv_label_set_text(diskLabel, buffer);

    if (tempC >= 0)
        snprintf(buffer, sizeof(buffer), "Temp   %.0f °C", tempC);
    else
        snprintf(buffer, sizeof(buffer), "Temp   --");

    if (strcmp(lv_label_get_text(tempLabel), buffer) != 0)
        lv_label_set_text(tempLabel, buffer);
}
