#include "SystemCard.h"
#include "../../../icons/Icons.h"
#include "../../themes/Theme.h"

#include <Arduino.h>

SystemCard::SystemCard(lv_obj_t* parent)
    : Card(parent)
{
    setClickScreen(ScreenType::System);
    
    title = lv_label_create(card);

    serverTitle = lv_label_create(card);
    serverCpuLabel = lv_label_create(card);
    serverRamLabel = lv_label_create(card);
    serverUptimeLabel = lv_label_create(card);
    
    tabletTitle = lv_label_create(card);
    tabletCpuLabel = lv_label_create(card);
    tabletRamLabel = lv_label_create(card);
    tabletUptimeLabel = lv_label_create(card);


    // -------------------------
    // Colors
    // -------------------------

    lv_obj_set_style_text_color(
        title,
        Theme::Text(),
        0
    );

    lv_obj_set_style_text_color(
        serverTitle,
        Theme::Text(),
        0
    );

    lv_obj_set_style_text_color(
        serverCpuLabel,
        Theme::Text(),
        0
    );

    lv_obj_set_style_text_color(
        serverRamLabel,
        Theme::Text(),
        0
    );

    lv_obj_set_style_text_color(
        serverUptimeLabel,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        tabletTitle,
        Theme::Text(),
        0
    );

    lv_obj_set_style_text_color(
        tabletCpuLabel,
        Theme::Text(),
        0
    );

    lv_obj_set_style_text_color(
        tabletRamLabel,
        Theme::Text(),
        0
    );

    lv_obj_set_style_text_color(
        tabletUptimeLabel,
        Theme::Text(),
        0
    );


    // -------------------------
    // Default text
    // -------------------------

    lv_label_set_text(
        title,
        "System"
    );

    lv_label_set_text(
        serverTitle,
        "SERVER"
    );

    lv_label_set_text(
        serverCpuLabel,
        "CPU    0.0 %"
    );

    lv_label_set_text(
        serverRamLabel,
        "RAM   0.0 %"
    );

    lv_label_set_text(
        tabletTitle,
        "TABLET"
    );

    lv_label_set_text(
        tabletCpuLabel,
        "CPU    0.0 %"
    );

    lv_label_set_text(
        tabletRamLabel,
        "RAM    0.0 %"
    );

    lv_label_set_text(
        tabletUptimeLabel,
        "UPTIME   0m"
    );
}

void SystemCard::layout()
{
    lv_obj_set_style_radius(root, 16, 0);

    // Dos columnas: servidor a la izquierda, tablet a partir de la mitad
    // de la card (con media anchura equivale a los 180 px de antes).
    const int serverX = 20;
    const int width = (cardWidth > 0) ? cardWidth : lv_obj_get_width(root);

    // Modo compacto (card de 1/3 de ancho, < 300 px): solo SERVIDOR en una
    // columna (CPU, RAM, uptime). Los datos de la tablet siguen en SystemScreen.
    if (width < 300)
    {
        layoutCompact();
        return;
    }

    // Con menos de 300 px usa la columna de siempre (180 px)
    const int tabletX = (width >= 300) ? (width / 2 + 10) : 180;

    // -------------------------
    // Icon chip
    // -------------------------

    lv_obj_t* iconChip = lv_image_create(root);
    lv_image_set_src(iconChip, &icon_system);
    lv_obj_align(iconChip, LV_ALIGN_TOP_LEFT, 0, 0);

    // -------------------------
    // Main title
    // -------------------------

    lv_obj_align(
        title,
        LV_ALIGN_TOP_LEFT,
        42,
        6
    );


    // -------------------------
    // SERVER
    // -------------------------

    lv_obj_align(
        serverTitle,
        LV_ALIGN_TOP_LEFT,
        serverX,
        36
    );

    lv_obj_align(
        serverCpuLabel,
        LV_ALIGN_TOP_LEFT,
        serverX,
        59
    );

    lv_obj_align(
        serverRamLabel,
        LV_ALIGN_TOP_LEFT,
        serverX,
        82
    );

    lv_obj_align(
        serverUptimeLabel,
        LV_ALIGN_TOP_LEFT,
        serverX,
        105
    );


    // -------------------------
    // TABLET
    // -------------------------

    lv_obj_align(
        tabletTitle,
        LV_ALIGN_TOP_LEFT,
        tabletX,
        36
    );

    lv_obj_align(
        tabletCpuLabel,
        LV_ALIGN_TOP_LEFT,
        tabletX,
        59
    );

    lv_obj_align(
        tabletRamLabel,
        LV_ALIGN_TOP_LEFT,
        tabletX,
        82
    );

    lv_obj_align(
        tabletUptimeLabel,
        LV_ALIGN_TOP_LEFT,
        tabletX,
        105
    );
}


void SystemCard::setServerCpu(double cpu)
{
    if (!serverCpuLabel)
        return;

    char buffer[32];

    snprintf(
        buffer,
        sizeof(buffer),
        "CPU    %.1f %%",
        cpu
    );

    lv_label_set_text(
        serverCpuLabel,
        buffer
    );
}


void SystemCard::setServerRam(double ram)
{
    if (!serverRamLabel)
        return;

    char buffer[32];

    snprintf(
        buffer,
        sizeof(buffer),
        "RAM   %.1f %%",
        ram
    );

    lv_label_set_text(
        serverRamLabel,
        buffer
    );
}


void SystemCard::setServerUptime(uint32_t uptime)
{
    if (!serverUptimeLabel)
        return;

    uint32_t days = uptime / 86400;
    uint32_t hours = (uptime % 86400) / 3600;
    uint32_t minutes = (uptime % 3600) / 60;

    // En modo compacto no hay sitio para la palabra UPTIME
    const char* prefix = compact ? "" : "UPTIME   ";

    char buffer[32];


    if (days > 0)
    {
        // Ejemplo: UPTIME   2d 04h 15m
        snprintf(
            buffer,
            sizeof(buffer),
            "%s%lud %02luh %02lum",
            prefix,
            (unsigned long)days,
            (unsigned long)hours,
            (unsigned long)minutes
        );
    }
    else if (hours > 0)
    {
        // Ejemplo: UPTIME   04h 15m
        snprintf(
            buffer,
            sizeof(buffer),
            "%s%luh %02lum",
            prefix,
            (unsigned long)hours,
            (unsigned long)minutes
        );
    }
    else
    {
        // Ejemplo: UPTIME   15m
        snprintf(
            buffer,
            sizeof(buffer),
            "%s%lum",
            prefix,
            (unsigned long)minutes
        );
    }

    lv_label_set_text(
        serverUptimeLabel,
        buffer
    );

}

// void SystemCard::setTabletCpu(
//     double cpu)
// {
//     if (!tabletCpuLabel)
//         return;


//     char buffer[32];


//     snprintf(
//         buffer,
//         sizeof(buffer),
//         "CPU    %.1f %%",
//         cpu
//     );


//     lv_label_set_text(
//         tabletCpuLabel,
//         buffer
//     );
// }


void SystemCard::setTabletRam(double ram)
{
    if (!tabletRamLabel)
        return;

    char buffer[32];

    snprintf(
        buffer,
        sizeof(buffer),
        "RAM    %.1f %%",
        ram
    );

    lv_label_set_text(
        tabletRamLabel,
        buffer
    );
}


void SystemCard::setTabletUptime(uint32_t uptime)
{
    if (!tabletUptimeLabel)
        return;

    // Cálculos del tiempo
    uint32_t days = uptime / 86400;
    uint32_t hours = (uptime % 86400) / 3600;
    uint32_t minutes = (uptime % 3600) / 60;

    char buffer[32];

    // Formateo condicional según el tiempo acumulado
    if (days > 0)
    {
        // Ejemplo: UPTIME   2d 04h 15m
        snprintf(
            buffer,
            sizeof(buffer),
            "UPTIME   %lud %02luh %02lum",
            (unsigned long)days,
            (unsigned long)hours,
            (unsigned long)minutes
        );
    }
    else if (hours > 0)
    {
        // Ejemplo: UPTIME   04h 15m
        snprintf(
            buffer,
            sizeof(buffer),
            "UPTIME   %luh %02lum",
            (unsigned long)hours,
            (unsigned long)minutes
        );
    }
    else
    {
        // Ejemplo: UPTIME   15m
        snprintf(
            buffer,
            sizeof(buffer),
            "UPTIME   %lum",
            (unsigned long)minutes
        );
    }

    lv_label_set_text(
        tabletUptimeLabel,
        buffer
    );
}


// Una sola columna con padding 20 (como CasaCard/ConfigCard)
void SystemCard::layoutCompact()
{
    compact = true;

    lv_obj_set_style_pad_all(root, PADDING, 0);
    lv_obj_set_style_radius(root, 16, 0);

    lv_obj_t* iconChip = lv_image_create(root);
    lv_image_set_src(iconChip, &icon_system);
    lv_obj_align(iconChip, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 42, 6);

    // Se oculta lo que no cabe
    lv_obj_add_flag(serverTitle, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tabletTitle, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tabletCpuLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tabletRamLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tabletUptimeLabel, LV_OBJ_FLAG_HIDDEN);

    lv_obj_align(serverCpuLabel, LV_ALIGN_TOP_LEFT, 0, 38);
    lv_obj_align(serverRamLabel, LV_ALIGN_TOP_LEFT, 0, 62);
    lv_obj_align(serverUptimeLabel, LV_ALIGN_TOP_LEFT, 0, 86);
}
