#include "DevicesScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../services/ServiceManager.h"
#include "../../models/DeviceInfo.h"

#include <Arduino.h>
#include <string.h>
#include <stdio.h>


// Area de contenido: 800 x 970. Titulo + 5 cards de 170 px.
static constexpr int GRID_TOP = ContentLayout::CONTENT_TOP;
static constexpr int CARD_HEIGHT = 170;
static constexpr int GAP = 10;

static constexpr int PAD = 12;
static constexpr int COLUMN_GAP = 20;
static constexpr int FIRST_LINE_Y = 30;   // bajo la cabecera (nombre + estado)
static constexpr int LINE_STEP = 19;      // fuente de 14 px (17) + 2


void DevicesScreen::create(lv_obj_t* parent)
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


void DevicesScreen::setText(lv_obj_t* label, const char* text)
{
    if (strcmp(lv_label_get_text(label), text) != 0)
        lv_label_set_text(label, text);
}


lv_obj_t* DevicesScreen::addLine(
    lv_obj_t* parent,
    int x,
    int y,
    int width,
    lv_color_t color
)
{
    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_text_font(label, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(label, color, 0);

    // Ancho fijo y una sola linea: si no cabe, termina en "..."
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_obj_set_size(label, width, 18);
    lv_obj_set_pos(label, x, y);

    return label;
}


DevicesScreen::DeviceCardUi DevicesScreen::createCard(
    lv_obj_t* parent,
    int x,
    int y,
    int width,
    int height
)
{
    DeviceCardUi ui;

    ui.box = lv_obj_create(parent);
    lv_obj_remove_style_all(ui.box);
    lv_obj_set_pos(ui.box, x, y);
    lv_obj_set_size(ui.box, width, height);
    lv_obj_clear_flag(ui.box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui.box, Theme::Panel(), 0);
    lv_obj_set_style_bg_opa(ui.box, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(ui.box, 16, 0);

    const int inner = width - PAD * 2;
    const int colWidth = (inner - COLUMN_GAP) / 2;
    const int rightX = PAD + colWidth + COLUMN_GAP;

    // Cabecera: nombre a la izquierda, estado a la derecha
    ui.name = lv_label_create(ui.box);
    lv_label_set_text(ui.name, "");
    lv_obj_set_style_text_font(ui.name, &montserrat_28_es_20, 0);
    lv_obj_set_style_text_color(ui.name, Theme::Text(), 0);
    lv_label_set_long_mode(ui.name, LV_LABEL_LONG_DOT);
    lv_obj_set_size(ui.name, colWidth, 24);
    lv_obj_set_pos(ui.name, PAD, PAD - 2);

    ui.status = lv_label_create(ui.box);
    lv_label_set_text(ui.status, "");
    lv_obj_set_style_text_font(ui.status, &montserrat_28_es_20, 0);
    lv_obj_set_style_text_color(ui.status, Theme::TextSecondary(), 0);

    // El estado ocupa TODA la mitad derecha de la cabecera (mismo ancho que
    // la columna derecha) con el texto CENTRADO ahi, y misma fuente y misma
    // posicion vertical que el nombre: queda alineado y lejos del borde.
    lv_obj_set_size(ui.status, colWidth, 24);
    lv_obj_set_style_text_align(ui.status, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(ui.status, rightX, PAD - 2);

    for (int i = 0; i < LINES; i++)
    {
        const int lineY = PAD + FIRST_LINE_Y + i * LINE_STEP - 4;

        ui.left[i]  = addLine(ui.box, PAD,    lineY, colWidth, Theme::TextSecondary());
        ui.right[i] = addLine(ui.box, rightX, lineY, colWidth, Theme::TextSecondary());
    }

    return ui;
}


void DevicesScreen::createContent()
{
    ContentLayout::createTitle(screen, "Dispositivos");

    const int left = ContentLayout::LEFT;
    const int fullWidth = ContentLayout::screenContentWidth(screen);

    for (int i = 0; i < CARD_COUNT; i++)
    {
        cards[i] = createCard(
            screen,
            left,
            GRID_TOP + i * (CARD_HEIGHT + GAP),
            fullWidth,
            CARD_HEIGHT
        );
    }

    emptyLabel = lv_label_create(screen);
    lv_label_set_text(emptyLabel, "Sin dispositivos");
    lv_obj_set_style_text_font(emptyLabel, &montserrat_28_es_20, 0);
    lv_obj_set_style_text_color(emptyLabel, Theme::TextSecondary(), 0);
    lv_obj_align(emptyLabel, LV_ALIGN_TOP_MID, 0, GRID_TOP + 20);
    lv_obj_add_flag(emptyLabel, LV_OBJ_FLAG_HIDDEN);
}


// 550e8410-e29b-41d4-a716-446655221111 -> 550e8410...1111
static void shortId(const String& id, char* out, size_t size)
{
    const unsigned int len = id.length();

    if (len <= 15)
    {
        snprintf(out, size, "%s", id.c_str());
        return;
    }

    snprintf(out, size, "%.8s...%s", id.c_str(), id.c_str() + (len - 4));
}

static void formatInterval(int seconds, char* out, size_t size)
{
    if (seconds <= 0)
        snprintf(out, size, "--");
    else if (seconds < 60)
        snprintf(out, size, "%d s", seconds);
    else if (seconds < 3600 && seconds % 60 == 0)
        snprintf(out, size, "%d min", seconds / 60);
    else if (seconds % 3600 == 0)
        snprintf(out, size, "%d h", seconds / 3600);
    else
        snprintf(out, size, "%d s", seconds);
}


void DevicesScreen::update()
{
    const DeviceInfo& devices = ServiceManager::api().devices();

    char buf[96];
    char idBuf[24];

    if (devices.deviceListCount == 0)
        lv_obj_clear_flag(emptyLabel, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(emptyLabel, LV_OBJ_FLAG_HIDDEN);

    for (int i = 0; i < CARD_COUNT; i++)
    {
        DeviceCardUi& ui = cards[i];

        if (i >= devices.deviceListCount)
        {
            lv_obj_add_flag(ui.box, LV_OBJ_FLAG_HIDDEN);
            continue;
        }

        lv_obj_clear_flag(ui.box, LV_OBJ_FLAG_HIDDEN);

        const DeviceEntryInfo& d = devices.devicesList[i];

        // ---- cabecera ----
        setText(ui.name, d.name.c_str());

        setText(ui.status, d.online ? "ONLINE" : "OFFLINE");
        lv_obj_set_style_text_color(
            ui.status,
            d.online ? Theme::Success() : Theme::Error(),
            0
        );

        // ---- columna izquierda: identidad ----
        shortId(d.id, idBuf, sizeof(idBuf));
        snprintf(buf, sizeof(buf), "Device ID: %s", idBuf);
        setText(ui.left[0], buf);

        if (d.hasSensor)
        {
            shortId(d.sensorId, idBuf, sizeof(idBuf));
            snprintf(buf, sizeof(buf), "Sensor ID: %s", idBuf);
        }
        else
        {
            snprintf(buf, sizeof(buf), "Sensor ID: --");
        }
        setText(ui.left[1], buf);

        snprintf(buf, sizeof(buf), "Desc: %s", d.description.c_str());
        setText(ui.left[2], buf);

        // "ESP32_C3" -> "ESP32 C3" + revision de la placa
        String model = d.deviceType;
        model.replace("_", " ");

        if (d.hardwareModel.length() > 0)
            snprintf(buf, sizeof(buf), "Modelo: %s (HW %s)", model.c_str(), d.hardwareModel.c_str());
        else
            snprintf(buf, sizeof(buf), "Modelo: %s", model.c_str());
        setText(ui.left[3], buf);

        snprintf(buf, sizeof(buf), "Tipo: %s", d.hasSensor ? d.sensorType.c_str() : "--");
        setText(ui.left[4], buf);

        snprintf(buf, sizeof(buf), "Ubicación: %s", d.location.c_str());
        setText(ui.left[5], buf);

        // ---- columna derecha: lectura y red ----
        if (d.hasReading)
            snprintf(buf, sizeof(buf), "Temp: %.1f °C", d.temperature);
        else
            snprintf(buf, sizeof(buf), "Temp: --");
        setText(ui.right[0], buf);

        if (d.humidityValid)
            snprintf(buf, sizeof(buf), "Humedad: %.0f %%", d.humidity);
        else
            snprintf(buf, sizeof(buf), "Humedad: --");
        setText(ui.right[1], buf);

        // La lectura destaca; atenuada si la medicion es antigua
        const lv_opa_t readingOpa = d.stale ? LV_OPA_50 : LV_OPA_COVER;

        for (int k = 0; k < 2; k++)
        {
            lv_obj_set_style_text_color(ui.right[k], Theme::Text(), 0);
            lv_obj_set_style_text_opa(ui.right[k], readingOpa, 0);
        }

        snprintf(buf, sizeof(buf), "IP: %s", d.ipAddress.c_str());
        setText(ui.right[2], buf);

        snprintf(buf, sizeof(buf), "MAC: %s", d.macAddress.c_str());
        setText(ui.right[3], buf);

        snprintf(buf, sizeof(buf), "Firmware: %s", d.firmwareVersion.c_str());
        setText(ui.right[4], buf);

        if (d.hasSensor)
        {
            char interval[16];
            formatInterval(d.sensorInterval, interval, sizeof(interval));
            snprintf(buf, sizeof(buf), "Intervalo: %s", interval);
        }
        else
        {
            snprintf(buf, sizeof(buf), "Intervalo: --");
        }
        setText(ui.right[5], buf);
    }
}


void DevicesScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);

    // MainLayout::setBody llama a show() con el mutex de datos tomado
    update();
}


void DevicesScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);
}


void DevicesScreen::destroy()
{
    if (screen)
    {
        lv_obj_delete(screen);
        screen = nullptr;
    }
}
