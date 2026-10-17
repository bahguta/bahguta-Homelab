#include "DockerCard.h"
#include "../../../icons/Icons.h"

#include "../../themes/Theme.h"
#include <Arduino.h>

DockerCard::DockerCard(lv_obj_t* parent)
    : Card(parent)
{
    setClickScreen(ScreenType::Docker);
}

void DockerCard::layout()
{
    lv_obj_set_style_pad_all(root, PADDING, 0);
    lv_obj_set_style_radius(root, 16, 0);

    iconChip = lv_image_create(root);
    lv_image_set_src(iconChip, &icon_docker);
    lv_obj_align(iconChip, LV_ALIGN_TOP_LEFT, 0, 0);

    titleLabel = lv_label_create(root);
    lv_label_set_text(titleLabel, "Docker");
    lv_obj_set_style_text_color(titleLabel, Theme::Text(), 0);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_LEFT, 42, 6);

    runningLabel = lv_label_create(root);
    lv_label_set_text(runningLabel, "0 / 0");
    lv_obj_set_style_text_color(runningLabel, Theme::Text(), 0);
    lv_obj_set_style_text_font(runningLabel, &lv_font_montserrat_28, 0);
    lv_obj_align(runningLabel, LV_ALIGN_TOP_LEFT, 0, 40);

    statusLabel = lv_label_create(root);
    lv_label_set_text(statusLabel, "Servicios activos");
    lv_obj_set_style_text_color(statusLabel, Theme::TextSecondary(), 0);
    lv_obj_align(statusLabel, LV_ALIGN_TOP_LEFT, 0, 76);

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

void DockerCard::setRunning(int running, int total)
{
    if (!runningLabel || !statusLabel || !barBg || !barFill)
        return;

    char buffer[32];

    snprintf(buffer, sizeof(buffer), "%d / %d", running, total);
    lv_label_set_text(runningLabel, buffer);

    int stopped = total - running;
    if (stopped < 0) stopped = 0;

    snprintf(buffer, sizeof(buffer), "%d activos, %d parados", running, stopped);
    lv_label_set_text(statusLabel, buffer);

    int pct = (total > 0) ? (running * 100 / total) : 0;
    int barWidth = lv_obj_get_width(barBg);
    if (barWidth > 0)
        lv_obj_set_width(barFill, (barWidth * pct) / 100);
}
