#include "SensorCard.h"
#include "../../../icons/Icons.h"
#include "../../themes/Theme.h"
#include <Arduino.h>

SensorCard::SensorCard(lv_obj_t* parent)
    : Card(parent)
{
    setClickScreen(ScreenType::Sensors);
}

void SensorCard::layout()
{
    lv_obj_set_style_pad_all(root, PADDING, 0);
    lv_obj_set_style_radius(root, 16, 0);

    iconChip = lv_image_create(root);
    lv_image_set_src(iconChip, &icon_sensors);
    lv_obj_align(iconChip, LV_ALIGN_TOP_LEFT, 0, 0);

    title = lv_label_create(root);
    lv_label_set_text(title, "Sensores");
    lv_obj_set_style_text_color(title, Theme::Text(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 42, 6);

    value = lv_label_create(root);
    lv_label_set_text(value, "0 / 0");
    lv_obj_set_style_text_color(value, Theme::Text(), 0);
    lv_obj_set_style_text_font(value, &lv_font_montserrat_28, 0);
    lv_obj_align(value, LV_ALIGN_TOP_LEFT, 0, 40);

    caption = lv_label_create(root);
    lv_label_set_text(caption, "Sensores online");
    lv_obj_set_style_text_color(caption, Theme::TextSecondary(), 0);
    lv_obj_align(caption, LV_ALIGN_TOP_LEFT, 0, 76);

    barBg = lv_obj_create(root);
    lv_obj_remove_style_all(barBg);
    lv_obj_set_size(barBg, LV_PCT(100), 6);
    lv_obj_set_style_bg_color(barBg, Theme::Background(), 0);
    lv_obj_set_style_bg_opa(barBg, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(barBg, 3, 0);
    lv_obj_align(barBg, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_clear_flag(barBg, LV_OBJ_FLAG_SCROLLABLE);

    barFill = lv_obj_create(barBg);
    lv_obj_remove_style_all(barFill);
    lv_obj_set_size(barFill, 0, 6);
    lv_obj_set_style_bg_color(barFill, Theme::Success(), 0);
    lv_obj_set_style_bg_opa(barFill, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(barFill, 3, 0);
    lv_obj_align(barFill, LV_ALIGN_LEFT_MID, 0, 0);
}

void SensorCard::setOnline(int online, int total)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d / %d", online, total);
    lv_label_set_text(value, buffer);

    int pct = (total > 0) ? (online * 100 / total) : 0;
    int barWidth = lv_obj_get_width(barBg);
    if (barWidth > 0)
        lv_obj_set_width(barFill, (barWidth * pct) / 100);
}
