#include "GrafanaScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../models/GrafanaInfo.h"
#include "../../services/ServiceManager.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>


void GrafanaScreen::create(lv_obj_t* parent)
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


void GrafanaScreen::setText(lv_obj_t* label, const char* text)
{
    if (strcmp(lv_label_get_text(label), text) != 0)
        lv_label_set_text(label, text);
}


void GrafanaScreen::createSection(lv_obj_t* parent, const char* text)
{
    lv_obj_t* header = lv_label_create(parent);
    lv_label_set_text(header, text);
    lv_obj_set_style_text_font(header, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(header, Theme::TextSecondary(), 0);
    lv_obj_set_style_pad_top(header, 14, 0);
}


lv_obj_t* GrafanaScreen::createRow(lv_obj_t* parent, const char* label)
{
    lv_obj_t* row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, LV_PCT(100), 40);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* labelObj = lv_label_create(row);
    lv_label_set_text(labelObj, label);
    lv_obj_set_style_text_font(labelObj, &montserrat_28_es_16, 0);
    lv_obj_set_style_text_color(labelObj, Theme::TextSecondary(), 0);
    lv_obj_align(labelObj, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* valueObj = lv_label_create(row);
    lv_label_set_text(valueObj, "--");
    lv_obj_set_style_text_font(valueObj, &montserrat_28_es_16, 0);
    lv_obj_set_style_text_color(valueObj, Theme::Text(), 0);
    lv_obj_align(valueObj, LV_ALIGN_RIGHT_MID, 0, 0);

    return valueObj;
}


GrafanaScreen::DiskView GrafanaScreen::createDisk(lv_obj_t* parent, const char* title)
{
    DiskView d;

    lv_obj_t* box = lv_obj_create(parent);
    lv_obj_remove_style_all(box);
    lv_obj_set_size(box, LV_PCT(100), 104);
    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);

    // Titulo (izquierda) y total (derecha)
    lv_obj_t* titleLabel = lv_label_create(box);
    lv_label_set_text(titleLabel, title);
    lv_obj_set_style_text_font(titleLabel, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(titleLabel, Theme::TextSecondary(), 0);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_LEFT, 0, 4);

    d.total = lv_label_create(box);
    lv_label_set_text(d.total, "--");
    lv_obj_set_style_text_font(d.total, &montserrat_28_es_16, 0);
    lv_obj_set_style_text_color(d.total, Theme::Text(), 0);
    lv_obj_align(d.total, LV_ALIGN_TOP_RIGHT, 0, 2);

    // Barra: parte ocupada (indicador) sobre fondo = espacio libre
    d.bar = lv_bar_create(box);
    lv_obj_set_size(d.bar, LV_PCT(100), 24);
    lv_obj_align(d.bar, LV_ALIGN_TOP_LEFT, 0, 34);
    lv_bar_set_range(d.bar, 0, 1000);
    lv_bar_set_value(d.bar, 0, LV_ANIM_OFF);

    lv_obj_set_style_radius(d.bar, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(d.bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(d.bar, Theme::Success(), LV_PART_MAIN);

    lv_obj_set_style_radius(d.bar, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(d.bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(d.bar, Theme::Primary(), LV_PART_INDICATOR);

    // Leyenda con los mismos colores que la barra
    d.used = lv_label_create(box);
    lv_label_set_text(d.used, "Ocupado  --");
    lv_obj_set_style_text_font(d.used, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(d.used, Theme::Primary(), 0);
    lv_obj_align(d.used, LV_ALIGN_TOP_LEFT, 0, 70);

    d.free = lv_label_create(box);
    lv_label_set_text(d.free, "Libre  --");
    lv_obj_set_style_text_font(d.free, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(d.free, Theme::Success(), 0);
    lv_obj_align(d.free, LV_ALIGN_TOP_RIGHT, 0, 70);

    return d;
}


void GrafanaScreen::createContent()
{
    ContentLayout::createTitle(screen, "Grafana");

    lv_obj_t* list = lv_obj_create(screen);
    lv_obj_remove_style_all(list);
    lv_obj_update_layout(screen);
    lv_obj_set_size(list, ContentLayout::screenContentWidth(screen),
                    lv_obj_get_height(screen) - ContentLayout::CONTENT_TOP - ContentLayout::TOP);
    lv_obj_set_pos(list, ContentLayout::LEFT, ContentLayout::CONTENT_TOP);
    lv_obj_set_style_bg_color(list, Theme::Panel(), 0);
    lv_obj_set_style_bg_opa(list, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(list, 16, 0);
    lv_obj_set_style_pad_all(list, 20, 0);
    lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

    rootDisk = createDisk(list, "DISCO  /  (SISTEMA)");
    storageDisk = createDisk(list, "DISCO  /srv/storage");

    createSection(list, "SERVIDOR");
    cpuValue  = createRow(list, "CPU");
    ramValue  = createRow(list, "Memoria");
    tempValue = createRow(list, "Temperatura");
}


static void formatGb(double gb, char* out, size_t size)
{
    if (gb < 0)
        snprintf(out, size, "--");
    else if (gb >= 1000.0)
        snprintf(out, size, "%.2f TB", gb / 1024.0);
    else
        snprintf(out, size, "%.1f GB", gb);
}

static void formatPercent(double v, char* out, size_t size)
{
    if (v < 0)
        snprintf(out, size, "--");
    else
        snprintf(out, size, "%.1f %%", v);
}


void GrafanaScreen::updateDisk(DiskView& d, double totalGb, double usedGb, double freeGb)
{
    char buf[48];
    char part[24];

    if (totalGb <= 0 || usedGb < 0)
    {
        lv_bar_set_value(d.bar, 0, LV_ANIM_OFF);
        setText(d.total, "--");
        setText(d.used, "Ocupado  --");
        setText(d.free, "Libre  --");
        return;
    }

    double pct = usedGb * 100.0 / totalGb;
    if (pct > 100.0) pct = 100.0;

    lv_bar_set_value(d.bar, (int32_t)(pct * 10.0), LV_ANIM_OFF);

    // Ocupado en azul; avisa en naranja (>= 75 %) y rojo (>= 90 %)
    lv_color_t color = pct >= 90.0 ? Theme::Error()
                     : pct >= 75.0 ? Theme::Warning()
                                   : Theme::Primary();
    lv_obj_set_style_bg_color(d.bar, color, LV_PART_INDICATOR);
    lv_obj_set_style_text_color(d.used, color, 0);

    formatGb(totalGb, part, sizeof(part));
    setText(d.total, part);

    formatGb(usedGb, part, sizeof(part));
    snprintf(buf, sizeof(buf), "Ocupado  %s (%.0f %%)", part, pct);
    setText(d.used, buf);

    formatGb(freeGb, part, sizeof(part));
    snprintf(buf, sizeof(buf), "Libre  %s", part);
    setText(d.free, buf);
}


void GrafanaScreen::update()
{
    if (!screen)
        return;

    const GrafanaInfo& g = ServiceManager::home().home().grafana;

    char buf[32];

    updateDisk(rootDisk, g.rootTotalGb, g.rootUsedGb, g.rootFreeGb);
    updateDisk(storageDisk, g.storageTotalGb, g.storageUsedGb, g.storageFreeGb);

    formatPercent(g.cpu, buf, sizeof(buf));      setText(cpuValue, buf);
    formatPercent(g.ram, buf, sizeof(buf));      setText(ramValue, buf);

    if (g.tempC < 0)
        snprintf(buf, sizeof(buf), "--");
    else
        snprintf(buf, sizeof(buf), "%.1f °C", g.tempC);
    setText(tempValue, buf);
}


void GrafanaScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);

    // MainLayout::setBody llama a show() con el mutex de datos tomado
    update();
}


void GrafanaScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);
}


void GrafanaScreen::destroy()
{
    if (screen)
    {
        lv_obj_delete(screen);
        screen = nullptr;
    }
}
