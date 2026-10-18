#include "RedScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../models/RedInfo.h"
#include "../../services/ServiceManager.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>


// Tabla: el panel mide el ancho de Home (760) y deja 20 px de relleno -> 720 utiles
static constexpr int PANEL_PAD = 20;
static constexpr int COUNT_Y = PANEL_PAD;          // "N equipos conectados"
static constexpr int HEADER_Y = PANEL_PAD + 34;    // cabecera de columnas
static constexpr int HEADER_H = 26;
static constexpr int LIST_Y = HEADER_Y + HEADER_H + 4;
static constexpr int ROW_H = 40;

// Columnas (x relativa al area util y ancho)
static constexpr int COL_NAME_X = 0,   COL_NAME_W = 250;
static constexpr int COL_TYPE_X = 250, COL_TYPE_W = 130;
static constexpr int COL_IP_X   = 380, COL_IP_W   = 160;
static constexpr int COL_MAC_X  = 540, COL_MAC_W  = 180;


static lv_obj_t* addCell(lv_obj_t* parent, const char* text, int x, int w,
                         const lv_font_t* font, lv_color_t color)
{
    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_obj_set_width(label, w);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, x, 0); 

    return label;
}


void RedScreen::create(lv_obj_t* parent)
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


void RedScreen::createContent()
{
    ContentLayout::createTitle(screen, "Red");

    lv_obj_update_layout(screen);

    // Panel unico con el ancho de Home, justo debajo del titulo
    lv_obj_t* panel = lv_obj_create(screen);
    lv_obj_remove_style_all(panel);
    lv_obj_set_size(panel, ContentLayout::screenContentWidth(screen), lv_obj_get_height(screen) - ContentLayout::CONTENT_TOP - ContentLayout::TOP);
    lv_obj_set_pos(panel, ContentLayout::LEFT, ContentLayout::CONTENT_TOP);
    lv_obj_set_style_bg_color(panel, Theme::Panel(), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(panel, 16, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    const int innerW = lv_obj_get_width(panel) - PANEL_PAD * 2;

    // "N equipos conectados" como titulo del bloque, a la izquierda
    countLabel = lv_label_create(panel);
    lv_label_set_text(countLabel, "Sin datos");
    lv_obj_set_style_text_font(countLabel, &montserrat_28_es_20, 0);
    lv_obj_set_style_text_color(countLabel, Theme::Text(), 0);
    lv_obj_set_pos(countLabel, PANEL_PAD, COUNT_Y);

    // Cabecera de columnas
    lv_obj_t* header = lv_obj_create(panel);
    lv_obj_remove_style_all(header);
    lv_obj_set_size(header, LV_PCT(100), HEADER_H); //innerW, HEADER_H);
    lv_obj_set_pos(header, PANEL_PAD, HEADER_Y);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(header, Theme::TextSecondary(), 0);

    const lv_font_t* hf = &montserrat_28_es_14;
    addCell(header, "Usuario conectado", COL_NAME_X, COL_NAME_W, hf, Theme::TextSecondary());
    addCell(header, "Tipo conexion",     COL_TYPE_X, COL_TYPE_W, hf, Theme::TextSecondary());
    addCell(header, "Direccion IP",      COL_IP_X,   COL_IP_W,   hf, Theme::TextSecondary());
    addCell(header, "MAC Address",       COL_MAC_X,  COL_MAC_W,  hf, Theme::TextSecondary());

    // Filas con scroll vertical (puede haber muchos equipos)
    list = lv_obj_create(panel);
    lv_obj_remove_style_all(list);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100)); //innerW, lv_obj_get_height(panel) - LIST_Y - PANEL_PAD);
    lv_obj_set_pos(list, PANEL_PAD, LIST_Y);

    lv_obj_set_style_pad_all(list, 0, 0);       // Quita paddings internos
    lv_obj_set_style_pad_row(list, 0, 0);       // Quita espacio automático entre filas flex
    lv_obj_set_style_pad_column(list, 0, 0);    // Quita espacios en columnas flex

    lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

    
}


// Firma barata del contenido para no reconstruir la lista si no cambio nada
static uint32_t signatureOf(const RedInfo& red)
{
    uint32_t h = 2166136261u;

    auto mix = [&h](const String& s)
    {
        for (size_t i = 0; i < s.length(); i++)
        {
            h ^= (uint8_t)s[i];
            h *= 16777619u;
        }
    };

    h ^= red.connected ? 1u : 0u;
    h *= 16777619u;
    h ^= (uint32_t)red.total;
    h *= 16777619u;

    for (const RedClientInfo& c : red.clients)
    {
        mix(c.name);
        mix(c.ip);
        mix(c.connection);
        mix(c.mac);
    }

    return h;
}


void RedScreen::rebuildList(const RedInfo& red)
{
    lv_obj_clean(list);

    int shown = 0;

    for (const RedClientInfo& c : red.clients)
    {
        if (shown >= MAX_ROWS)
            break;

        lv_obj_t* row = lv_obj_create(list);
        lv_obj_remove_style_all(row);
        lv_obj_set_size(row, LV_PCT(100), ROW_H);

        lv_obj_set_style_pad_all(row, 0, 0);       // Asegura que no tenga relleno interno
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE); // Evita que la propia fila intente hacer scroll

        lv_obj_set_style_border_width(row, 1, 0);
        lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0);
        lv_obj_set_style_border_color(row, Theme::Background(), 0);

        const lv_font_t* bf = &montserrat_28_es_14;

        // Usuario (o MAC si el router no da nombre)
        addCell(row, c.name.length() > 0 ? c.name.c_str() : c.mac.c_str(),
                COL_NAME_X, COL_NAME_W, bf, Theme::Text());

        // Tipo de conexion, en su propia columna y centrado en la fila
        addCell(row, c.connection.length() > 0 ? c.connection.c_str() : "--",
                COL_TYPE_X, COL_TYPE_W, bf, Theme::Text());

        addCell(row, c.ip.length() > 0 ? c.ip.c_str() : "--",
                COL_IP_X, COL_IP_W, bf, Theme::Text());

        addCell(row, c.mac.length() > 0 ? c.mac.c_str() : "--",
                COL_MAC_X, COL_MAC_W, bf, Theme::Text());

        shown++;
    }

    if (shown == 0)
    {
        lv_obj_t* empty = lv_label_create(list);
        lv_label_set_text(empty, red.connected ? "Ningun equipo conectado" : "Sin datos del router");
        lv_obj_set_style_text_font(empty, &montserrat_28_es_16, 0);
        lv_obj_set_style_text_color(empty, Theme::TextSecondary(), 0);
    }
}


void RedScreen::update()
{
    if (!screen)
        return;

    const RedInfo& red = ServiceManager::home().home().red;

    char buf[48];

    if (red.connected)
        snprintf(buf, sizeof(buf), "%d equipos conectados", red.total);
    else
        snprintf(buf, sizeof(buf), "Sin datos");

    if (strcmp(lv_label_get_text(countLabel), buf) != 0)
        lv_label_set_text(countLabel, buf);

    const uint32_t sig = signatureOf(red);

    if (sig != lastSignature)
    {
        lastSignature = sig;
        rebuildList(red);
    }
}


void RedScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);

    // MainLayout::setBody llama a show() con el mutex de datos tomado
    update();
}


void RedScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);
}


void RedScreen::destroy()
{
    if (screen)
    {
        lv_obj_delete(screen);
        screen = nullptr;
    }
}
