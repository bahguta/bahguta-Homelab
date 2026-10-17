#include "SensorsScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../services/ServiceManager.h"
#include "../../models/SensorInfo.h"

#include <Arduino.h>


static constexpr int TITLE_TOP = 25;
static constexpr int GRID_TOP = 95;
static constexpr int ROW_HEIGHT = 260;
static constexpr int GAP = 15;
static constexpr int BODY_WIDTH = 800;


void SensorsScreen::create(lv_obj_t* parent)
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


lv_obj_t* SensorsScreen::addLine(lv_obj_t* parent, lv_color_t color, const lv_font_t* font)
{
    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, "--");
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_width(label, LV_PCT(100));
    return label;
}


SensorsScreen::SensorCardUi SensorsScreen::createCard(
    lv_obj_t* parent,
    int x,
    int y,
    int width,
    int height
)
{
    SensorCardUi result;

    lv_obj_t* box = lv_obj_create(parent);
    lv_obj_remove_style_all(box);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_size(box, width, height);
    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(box, Theme::Panel(), 0);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(box, 16, 0);
    lv_obj_set_style_pad_all(box, 14, 0);
    lv_obj_set_style_pad_row(box, 4, 0);

    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);

    result.name = addLine(box, Theme::Text(), &montserrat_28_es_24);
    result.enabled = addLine(box, Theme::TextSecondary(), &montserrat_28_es_16);
    result.type = addLine(box, Theme::TextSecondary(), &montserrat_28_es_16);
    result.interval = addLine(box, Theme::TextSecondary(), &montserrat_28_es_16);
    result.id = addLine(box, Theme::TextSecondary(), &montserrat_28_es_16);
    result.deviceId = addLine(box, Theme::TextSecondary(), &montserrat_28_es_16);

    return result;
}


void SensorsScreen::createContent()
{
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, "SENSORES");
    lv_obj_set_style_text_font(title, &montserrat_28_es_28, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, TITLE_TOP);
    lv_obj_set_style_text_color(title, Theme::Text(), 0);

    const int left = ContentLayout::LEFT;
    const int colWidth = ContentLayout::cardWidth(BODY_WIDTH);
    const int fullWidth = ContentLayout::contentWidth(BODY_WIDTH);
    const int col2 = left + colWidth + GAP;

    cards[0] = createCard(screen, left, GRID_TOP, colWidth, ROW_HEIGHT);
    cards[1] = createCard(screen, col2, GRID_TOP, colWidth, ROW_HEIGHT);

    const int row2Top = GRID_TOP + ROW_HEIGHT + GAP;
    cards[2] = createCard(screen, left, row2Top, colWidth, ROW_HEIGHT);
    cards[3] = createCard(screen, col2, row2Top, colWidth, ROW_HEIGHT);

    const int row3Top = row2Top + ROW_HEIGHT + GAP;
    cards[4] = createCard(screen, left, row3Top, fullWidth, ROW_HEIGHT);
}


void SensorsScreen::update()
{
    const SensorInfo& sensors = ServiceManager::api().sensors();

    char buf[64];

    for (int i = 0; i < CARD_COUNT; i++)
    {
        if (i < sensors.sensorListCount)
        {
            const SensorEntryInfo& entry = sensors.sensorsList[i];

            lv_label_set_text(cards[i].name, entry.name.c_str());

            lv_label_set_text(
                cards[i].enabled,
                entry.enabled ? "Habilitado: Sí" : "Habilitado: No");

            lv_obj_set_style_text_color(
                cards[i].enabled,
                entry.enabled ? Theme::Success() : Theme::Error(),
                0);

            snprintf(buf, sizeof(buf), "Tipo: %s", entry.type.c_str());
            lv_label_set_text(cards[i].type, buf);

            snprintf(buf, sizeof(buf), "Intervalo: %d s", entry.interval);
            lv_label_set_text(cards[i].interval, buf);

            String shortId = entry.id;
            if (shortId.length() > 8)
                shortId = shortId.substring(0, 8) + "...";

            snprintf(buf, sizeof(buf), "ID: %s", shortId.c_str());
            lv_label_set_text(cards[i].id, buf);

            String shortDeviceId = entry.deviceId;
            if (shortDeviceId.length() > 8)
                shortDeviceId = shortDeviceId.substring(0, 8) + "...";

            snprintf(buf, sizeof(buf), "Device ID: %s", shortDeviceId.c_str());
            lv_label_set_text(cards[i].deviceId, buf);
        }
        else
        {
            lv_label_set_text(cards[i].name, "--");
            lv_label_set_text(cards[i].enabled, "");
            lv_label_set_text(cards[i].type, "");
            lv_label_set_text(cards[i].interval, "");
            lv_label_set_text(cards[i].id, "");
            lv_label_set_text(cards[i].deviceId, "");
        }
    }
}


void SensorsScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);
    update();
}


void SensorsScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);
}


void SensorsScreen::destroy()
{
    if (screen)
    {
        lv_obj_delete(screen);
        screen = nullptr;
    }
}