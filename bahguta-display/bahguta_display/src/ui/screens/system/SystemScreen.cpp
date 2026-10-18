#include "SystemScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../services/ServiceManager.h"

#include <Arduino.h>
#include <string.h>
#include <stdio.h>


void SystemScreen::create(lv_obj_t* parent)
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


void SystemScreen::setText(lv_obj_t* label, const char* text)
{
    // Solo toca LVGL si el texto cambio (evita redibujar sin necesidad)
    if (strcmp(lv_label_get_text(label), text) != 0)
        lv_label_set_text(label, text);
}


void SystemScreen::createHeader(lv_obj_t* parent, const char* text)
{
    lv_obj_t* header = lv_label_create(parent);
    lv_label_set_text(header, text);
    lv_obj_set_style_text_font(header, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(header, Theme::TextSecondary(), 0);
    lv_obj_set_style_pad_top(header, 14, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
}


lv_obj_t* SystemScreen::createRow(lv_obj_t* parent, const char* label)
{
    lv_obj_t* row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, LV_PCT(100), 44);
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


void SystemScreen::createContent()
{
    ContentLayout::createTitle(screen, "Sistema");

    lv_obj_t* list = lv_obj_create(screen);
    lv_obj_remove_style_all(list);
    // Mismo ancho que Home, justo debajo del titulo y hasta el final de la pantalla
    lv_obj_set_size(list, ContentLayout::screenContentWidth(screen),
                    lv_obj_get_height(screen) - ContentLayout::CONTENT_TOP - ContentLayout::TOP);
    lv_obj_set_pos(list, ContentLayout::LEFT, ContentLayout::CONTENT_TOP);
    lv_obj_set_style_bg_color(list, Theme::Panel(), 0);
    lv_obj_set_style_bg_opa(list, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(list, 16, 0);
    lv_obj_set_style_pad_all(list, 20, 0);
    lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

    createHeader(list, "SERVIDOR");
    serverUptimeValue = createRow(list, "Uptime");
    serverCpuValue    = createRow(list, "CPU");
    serverRamValue    = createRow(list, "RAM");

    createHeader(list, "TABLET");
    tabletUptimeValue = createRow(list, "Uptime");
    tabletRamValue    = createRow(list, "RAM");

    createHeader(list, "CONEXIÓN");
    backendUrlValue = createRow(list, "URL del backend");
    deviceNameValue = createRow(list, "Nombre del dispositivo");
    timezoneValue   = createRow(list, "Zona horaria");
}


static void formatUptime(double seconds, char* out, size_t size)
{
    const uint32_t s = (seconds > 0) ? (uint32_t)seconds : 0;

    const uint32_t days = s / 86400;
    const uint32_t hours = (s % 86400) / 3600;
    const uint32_t minutes = (s % 3600) / 60;

    if (days > 0)
        snprintf(out, size, "%ud %02uh %02um", (unsigned)days, (unsigned)hours, (unsigned)minutes);
    else
        snprintf(out, size, "%02uh %02um", (unsigned)hours, (unsigned)minutes);
}


void SystemScreen::update()
{
    char buf[64];

    // ---- servidor ----
    const BackendServerInfo& server = ServiceManager::api().server();

    if (server.connected)
    {
        formatUptime(server.uptime, buf, sizeof(buf));
        setText(serverUptimeValue, buf);

        snprintf(buf, sizeof(buf), "%.1f %%", server.cpu);
        setText(serverCpuValue, buf);

        snprintf(buf, sizeof(buf), "%.1f %%", server.ram);
        setText(serverRamValue, buf);
    }
    else
    {
        setText(serverUptimeValue, "--");
        setText(serverCpuValue, "--");
        setText(serverRamValue, "--");
    }

    // ---- tablet ----
    const SystemInfo& tablet = ServiceManager::system().system();

    formatUptime((double)tablet.uptime, buf, sizeof(buf));
    setText(tabletUptimeValue, buf);

    snprintf(buf, sizeof(buf), "%.1f %%", tablet.ram);
    setText(tabletRamValue, buf);

    // ---- conexion ----
    const SettingsInfo& local = ServiceManager::settings().settings();
    const SettingsInfo& backend = ServiceManager::api().settings();

    setText(backendUrlValue, local.backendUrl.length() > 0 ? local.backendUrl.c_str() : "--");

    // El nombre viene de la configuracion del servidor; si aun no llego,
    // el guardado en la tablet
    if (backend.deviceName.length() > 0)
        setText(deviceNameValue, backend.deviceName.c_str());
    else
        setText(deviceNameValue, local.deviceName.length() > 0 ? local.deviceName.c_str() : "--");

    setText(timezoneValue, local.timezone.length() > 0 ? local.timezone.c_str() : "--");
}


void SystemScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);

    // MainLayout::setBody llama a show() con el mutex de datos tomado
    update();
}


void SystemScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);
}


void SystemScreen::destroy()
{
    if (screen)
    {
        lv_obj_delete(screen);
        screen = nullptr;
    }
}
