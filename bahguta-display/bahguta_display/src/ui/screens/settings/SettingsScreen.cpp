#include "SettingsScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../services/ServiceManager.h"

#include <Arduino.h>
#include <string.h>
#include <stdio.h>

void SettingsScreen::create(lv_obj_t* parent)
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

lv_obj_t* SettingsScreen::createRow(lv_obj_t* parent, const char* label)
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

lv_obj_t* SettingsScreen::createInputRow(lv_obj_t* parent, const char* label, bool password)
{
    lv_obj_t* row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, LV_PCT(100), 54);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* labelObj = lv_label_create(row);
    lv_label_set_text(labelObj, label);
    lv_obj_set_style_text_font(labelObj, &montserrat_28_es_16, 0);
    lv_obj_set_style_text_color(labelObj, Theme::TextSecondary(), 0);
    lv_obj_align(labelObj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_clear_flag(labelObj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* input = lv_textarea_create(row);
    lv_textarea_set_one_line(input, true);
    lv_textarea_set_password_mode(input, password);
    lv_obj_set_size(input, 230, 44);
    lv_obj_set_style_radius(input, 12, 0);
    lv_obj_align(input, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_font(input, &montserrat_28_es_16, 0);
    lv_obj_clear_flag(input, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(
        input,
        [](lv_event_t* e)
        {
            SettingsScreen* self =
                static_cast<SettingsScreen*>(lv_event_get_user_data(e));

            lv_obj_t* ta =
                static_cast<lv_obj_t*>(lv_event_get_target(e));

            lv_keyboard_set_textarea(self->keyboard, ta);
            lv_obj_clear_flag(self->keyboard, LV_OBJ_FLAG_HIDDEN);
        },
        LV_EVENT_FOCUSED,
        this
    );

    return input;
}

void SettingsScreen::createContent()
{
    ContentLayout::createTitle(screen, "Settings");

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
    lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);


    // ============================================================
    // RED (solo lectura, en vivo)
    // ============================================================

    lv_obj_t* netHeader = lv_label_create(list);
    lv_label_set_text(netHeader, "RED");
    lv_obj_set_style_text_font(netHeader, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(netHeader, Theme::TextSecondary(), 0);
    lv_obj_clear_flag(netHeader, LV_OBJ_FLAG_SCROLLABLE);

    ssidValue   = createRow(list, "Red conectada");
    signalValue = createRow(list, "Nivel de señal");
    ipValue     = createRow(list, "Dirección IP");


    // ============================================================
    // WIFI (editable)
    // ============================================================

    lv_obj_t* wifiHeader = lv_label_create(list);
    lv_label_set_text(wifiHeader, "CAMBIAR DE WIFI");
    lv_obj_set_style_text_font(wifiHeader, &montserrat_28_es_14, 0);
    lv_obj_set_style_text_color(wifiHeader, Theme::TextSecondary(), 0);
    lv_obj_set_style_pad_top(wifiHeader, 14, 0);
    lv_obj_clear_flag(wifiHeader, LV_OBJ_FLAG_SCROLLABLE);

    wifiSsidInput     = createInputRow(list, "Red WiFi (SSID)", false);
    wifiPasswordInput = createInputRow(list, "Contraseña WiFi", true);

    lv_obj_t* saveBtn = lv_button_create(list);
    lv_obj_set_size(saveBtn, LV_PCT(100), 60);
    lv_obj_set_style_bg_color(saveBtn, Theme::Primary(), 0);
    lv_obj_set_style_radius(saveBtn, 16, 0);
    lv_obj_set_style_margin_top(saveBtn, 10, 0);

    lv_obj_t* saveLabel = lv_label_create(saveBtn);
    lv_label_set_text(saveLabel, "Guardar y reconectar");
    lv_obj_set_style_text_font(saveLabel, &montserrat_28_es_18, 0);
    lv_obj_center(saveLabel);

    lv_obj_add_event_cb(
        saveBtn,
        [](lv_event_t* e)
        {
            SettingsScreen* self =
                static_cast<SettingsScreen*>(lv_event_get_user_data(e));

            SettingsInfo& s = ServiceManager::settings().settings();

            s.wifiSsid = lv_textarea_get_text(self->wifiSsidInput);
            s.wifiPassword = lv_textarea_get_text(self->wifiPasswordInput);

            ServiceManager::settings().save();
            ServiceManager::network().reconnect();

            lv_obj_add_flag(self->keyboard, LV_OBJ_FLAG_HIDDEN);
        },
        LV_EVENT_CLICKED,
        this
    );


    // ============================================================
    // APAGAR / REINICIAR LA TABLET
    // ============================================================

    lv_obj_t* actions = lv_obj_create(list);
    lv_obj_remove_style_all(actions);
    lv_obj_set_size(actions, LV_PCT(100), 80);
    lv_obj_clear_flag(actions, LV_OBJ_FLAG_SCROLLABLE);

    // Fuera del flujo de la lista y anclado al FONDO del cuadro, lejos del
    // boton de guardar el WiFi
    lv_obj_add_flag(actions, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align(actions, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_set_flex_flow(actions, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(actions, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* shutdownBtn = lv_button_create(actions);
    lv_obj_set_size(shutdownBtn, 190, 70);
    lv_obj_set_style_bg_color(shutdownBtn, Theme::Error(), 0);
    lv_obj_set_style_radius(shutdownBtn, 16, 0);

    lv_obj_add_event_cb(
        shutdownBtn,
        [](lv_event_t*)
        {
            ServiceManager::power().shutdown();
        },
        LV_EVENT_CLICKED,
        nullptr
    );

    lv_obj_t* shutdownLabel = lv_label_create(shutdownBtn);
    lv_label_set_text(shutdownLabel, "Apagar tablet");
    lv_obj_set_style_text_font(shutdownLabel, &montserrat_28_es_20, 0);
    lv_obj_center(shutdownLabel);

    lv_obj_t* restartBtn = lv_button_create(actions);
    lv_obj_set_size(restartBtn, 190, 70);
    lv_obj_set_style_bg_color(restartBtn, Theme::Primary(), 0);
    lv_obj_set_style_radius(restartBtn, 16, 0);

    lv_obj_add_event_cb(
        restartBtn,
        [](lv_event_t*)
        {
            Serial.println("Reiniciando tablet (boton Ajustes)...");
            ESP.restart();
        },
        LV_EVENT_CLICKED,
        nullptr
    );

    lv_obj_t* restartLabel = lv_label_create(restartBtn);
    lv_label_set_text(restartLabel, "Reiniciar");
    lv_obj_set_style_text_font(restartLabel, &montserrat_28_es_20, 0);
    lv_obj_center(restartLabel);


    // ============================================================
    // TECLADO (oculto hasta que se toque un campo editable)
    // ============================================================

    keyboard = lv_keyboard_create(screen);
    lv_obj_set_size(keyboard, LV_PCT(100), 240);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(
        keyboard,
        [](lv_event_t* e)
        {
            SettingsScreen* self =
                static_cast<SettingsScreen*>(lv_event_get_user_data(e));

            lv_obj_add_flag(self->keyboard, LV_OBJ_FLAG_HIDDEN);
        },
        LV_EVENT_READY,
        this
    );

    lv_obj_add_event_cb(
        keyboard,
        [](lv_event_t* e)
        {
            SettingsScreen* self =
                static_cast<SettingsScreen*>(lv_event_get_user_data(e));

            lv_obj_add_flag(self->keyboard, LV_OBJ_FLAG_HIDDEN);
        },
        LV_EVENT_CANCEL,
        this
    );
}

static const char* signalQuality(int rssi)
{
    if (rssi >= -55) return "Excelente";
    if (rssi >= -65) return "Buena";
    if (rssi >= -75) return "Aceptable";
    if (rssi >= -85) return "Débil";
    return "Muy débil";
}

static void setLabel(lv_obj_t* label, const char* text)
{
    if (strcmp(lv_label_get_text(label), text) != 0)
        lv_label_set_text(label, text);
}

void SettingsScreen::refreshLive()
{
    const NetworkInfo& n = ServiceManager::network().network();

    char buf[48];

    if (!n.connected)
    {
        setLabel(ssidValue, "Sin conexión");
        setLabel(signalValue, "--");
        setLabel(ipValue, "--");
        return;
    }

    setLabel(ssidValue, n.ssid.length() > 0 ? n.ssid.c_str() : "--");

    snprintf(buf, sizeof(buf), "%d dBm (%s)", n.rssi, signalQuality(n.rssi));
    setLabel(signalValue, buf);

    setLabel(ipValue, n.ip.length() > 0 ? n.ip.c_str() : "--");
}

void SettingsScreen::update()
{
    SettingsInfo& s = ServiceManager::settings().settings();

    refreshLive();

    lv_textarea_set_text(wifiSsidInput, s.wifiSsid.c_str());
    lv_textarea_set_text(wifiPasswordInput, s.wifiPassword.c_str());
}

void SettingsScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);
    update();
}

void SettingsScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);

    if (keyboard)
    {
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

void SettingsScreen::destroy()
{
    if (screen)
    {
        lv_obj_delete(screen);
        screen = nullptr;
    }
}