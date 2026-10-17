#include "Header.h"
#include "../../../fonts/Fonts.h"
#include "../../themes/Theme.h"
#include "../../utils/DateUtils.h"
#include "../../models/ClockInfo.h"
#include "../../services/ServiceManager.h"

Header::Header(lv_obj_t *parent)
{
    root = lv_obj_create(parent);

    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_width(root, LV_PCT(100));
    lv_obj_set_height(root, 70);

    lv_obj_set_pos(root,0,0);

    lv_obj_set_style_radius(root,0,0);

    lv_obj_set_style_border_width(root,0,0);

    lv_obj_set_style_bg_color(
        root,
        Theme::Header(),
        0);

    lv_obj_align(root, LV_ALIGN_TOP_MID, 0, 0);


    //
    // Logo
    //

    logo = lv_label_create(root);

    lv_label_set_recolor(logo,true);

    lv_label_set_text(
        logo,
        "#6DDC36 bahguta# #FFFFFF Homelab#");

    lv_obj_set_style_text_font(
        logo,
        &montserrat_28_es_24,
        0);

    lv_obj_align(
        logo,
        LV_ALIGN_LEFT_MID,
        20,
        0);


    //
    // Backend
    //

    backend = lv_label_create(root);

    lv_label_set_text(
        backend,
        LV_SYMBOL_HOME);

    lv_obj_align(
        backend,
        LV_ALIGN_RIGHT_MID,
        -190,
        0);

    lv_obj_set_style_text_color(
        backend,
        Theme::Text(),
        0);

    //
    // Docker
    //

    docker = lv_label_create(root);

    lv_label_set_text(
        docker,
        LV_SYMBOL_DIRECTORY);

    lv_obj_align(
        docker,
        LV_ALIGN_RIGHT_MID,
        -160,
        0);

    lv_obj_set_style_text_color(
        docker,
        Theme::Text(),
        0);

    //
    // WIFI
    //

    wifi = lv_label_create(root);

    lv_label_set_text(
        wifi,
        LV_SYMBOL_WIFI);

    lv_obj_align(
        wifi,
        LV_ALIGN_RIGHT_MID,
        -130,
        0);

    lv_obj_set_style_text_color(
        wifi,
        Theme::Text(),
        0);

    //
    // Battery
    //
    battery = lv_label_create(root);

    lv_label_set_text(battery, "--%");

    lv_obj_align(
        battery,
        LV_ALIGN_RIGHT_MID,
        -80,
        0);

    lv_obj_set_style_text_color(
        battery,
        Theme::Text(),
        0);

    //
    // Hora
    //
    clock = lv_label_create(root);

    lv_label_set_text(clock, "--:--");

    lv_obj_set_style_text_font(
        clock,
        &montserrat_28_es_20,
        0);

    lv_obj_align(
        clock,
        LV_ALIGN_RIGHT_MID,
        -20,
        0);

    lv_obj_set_style_text_color(
        clock,
        Theme::Text(),
        0);
}

void Header::setWifi(bool connected)
{
    lv_label_set_text(
        wifi, 
        (connected ? String("#6DDC36 ") + LV_SYMBOL_WIFI + "#" 
               : String("#FF4040 ") + LV_SYMBOL_WIFI + "#").c_str()
    );


    lv_label_set_recolor(wifi,true);
}

void Header::setBackend(bool connected)
{
    lv_label_set_text(
        backend,
        (connected ? String("#6DDC36 ") + LV_SYMBOL_HOME + "#" 
               : String("#FF4040 ") + LV_SYMBOL_HOME + "#").c_str()
    );
    lv_label_set_recolor(backend,true);
}

void Header::setDocker(bool connected)
{
    lv_label_set_text(
        docker, 
        (connected ? String("#6DDC36 ") + LV_SYMBOL_DIRECTORY + "#" 
               : String("#FF4040 ") + LV_SYMBOL_DIRECTORY + "#").c_str()
    );
    lv_label_set_recolor(docker,true);
}

void Header::layout()
{
    lv_obj_set_pos(root,0,0);

    lv_obj_set_size(
        root,
        800,
        70);
}


void Header::setTime()
{
    const ClockInfo& clock =
        ServiceManager::clock().clock();

    if(!clock.valid)
    {
        lv_label_set_text(this->clock,"--:--");
        return;
    }

    char buffer[6]; 

    sprintf(
        buffer,
        "%02d:%02d",
        clock.hour,
        clock.minute);

    lv_label_set_text(
        this->clock,
        buffer);
}

void Header::update()
{
    setTime();
    setWifi(ServiceManager::network().network().connected);
    setBackend(ServiceManager::api().api().connected);
    setBattery(ServiceManager::system().system().battery);
    setDocker(ServiceManager::api().docker().connected);
}

void Header::setBattery(int percent)
{
    char txt[10];

    if(percent < 0)
    {
        strcpy(txt,"--%");
    }
    else
    {
        sprintf(txt,"%d%%",percent);
    }

    lv_label_set_text(battery,txt);
}