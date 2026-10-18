#include "Footer.h"
#include "../../themes/Theme.h"
#include "../../models/ClockInfo.h"
#include "../../services/ServiceManager.h"
#include <ctime>

Footer::Footer(lv_obj_t *parent)
{
    root = lv_obj_create(parent);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_width(root, LV_PCT(100));
    lv_obj_set_height(root, 35);

    lv_obj_align(root,
                LV_ALIGN_BOTTOM_MID,
                0,
                0);
                
    lv_obj_set_style_radius(root,0,0);
    lv_obj_set_style_border_width(root,0,0);

    lv_obj_set_style_bg_color(
        root,
        Theme::Header(),
        0);

    //
    // Fecha
    //

    date = lv_label_create(root);

    //lv_label_set_text(date,"01 Ago 2026");

    lv_obj_set_style_text_color(
        date,
        Theme::Text(),
        0);

    lv_obj_align(
        date,
        LV_ALIGN_LEFT_MID,
        20,
        0);

    
    //
    // Developer
    //

    developer = lv_label_create(root);

    lv_label_set_text(
        developer,
        "Developed by Plamen"
    );

    lv_obj_set_style_text_color(
        developer,
        lv_color_hex(0x6DDC36),
        0
    );

    lv_obj_align(
        developer,
        LV_ALIGN_CENTER,
        0,
        0
    );

    //
    // Version
    //

    version = lv_label_create(root);

    lv_label_set_text(version,"v--");

    lv_obj_set_style_text_color(
        version,
        Theme::Text(),
        0);

    lv_obj_align(
        version,
        LV_ALIGN_RIGHT_MID,
        -20,
        0);
}

lv_obj_t *Footer::object()
{
    return root;
}

void Footer::setDate()
{
    const ClockInfo& clock =
        ServiceManager::clock().clock();

    if(!clock.valid) {
        lv_label_set_text(date,"--/--/----");
        return;
    }

    char buffer[30];

    snprintf(
        buffer,
        sizeof(buffer),
        "%02d/%02d/%04d",
        clock.day,
        clock.month,
        clock.year);
    
    lv_label_set_text(date,buffer);
    
}

void Footer::setVersion(const char *text)
{
    lv_label_set_text(version,text);
}

// void Footer::layout()
// {
//     lv_obj_set_pos(
//         root,
//         0,
//         1245);

//     lv_obj_set_size(
//         root,
//         800,
//         35);
// }

void Footer::layout()
{
    lv_obj_set_pos(
        root,
        0,
        1245
    );

    lv_obj_set_size(
        root,
        800,
        HEIGHT
    );
}

void Footer::update()
{
    setDate();

    // Version del proyecto, la que informa el backend (MainLayout::update
    // llama con el mutex de datos tomado). Solo se reescribe si cambia.
    const char* v = ServiceManager::api().server().version;

    if (v[0] && strcmp(v, shownVersion) != 0)
    {
        strlcpy(shownVersion, v, sizeof(shownVersion));

        char buffer[24];
        snprintf(buffer, sizeof(buffer), "v%s", shownVersion);
        lv_label_set_text(version, buffer);
    }
}