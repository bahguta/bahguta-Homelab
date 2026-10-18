

#include "BottomMenu.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/MainLayout.h"

#include "BottomMenu.h"

#include "../../themes/Theme.h"
#include "../../layout/MainLayout.h"

BottomMenu::BottomMenu(lv_obj_t* parent)
{
    root = lv_obj_create(parent);

    lv_obj_remove_style_all(root);

    lv_obj_clear_flag(
        root,
        LV_OBJ_FLAG_SCROLLABLE
    );

    lv_obj_set_size(
        root,
        LV_PCT(100),
        HEIGHT
    );

    lv_obj_align(
        root,
        LV_ALIGN_BOTTOM_MID,
        0,
        -35
    );

    lv_obj_set_style_radius(
        root,
        0,
        0
    );

    lv_obj_set_style_border_width(
        root,
        0,
        0
    );

    lv_obj_set_style_bg_color(
        root,
        Theme::Panel(),
        0
    );

    lv_obj_set_style_bg_opa(
        root,
        LV_OPA_COVER,
        0
    );


    // -------------------------------------------------
    // Container
    // -------------------------------------------------

    container = lv_obj_create(root);

    lv_obj_remove_style_all(container);

    lv_obj_clear_flag(
        container,
        LV_OBJ_FLAG_SCROLLABLE
    );

    lv_obj_set_size(
        container,
        LV_PCT(100),
        LV_PCT(100)
    );

    lv_obj_center(container);

    lv_obj_set_flex_flow(
        container,
        LV_FLEX_FLOW_ROW
    );

    lv_obj_set_flex_align(
        container,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    lv_obj_set_style_pad_column(
        container,
        15,
        0
    );

    lv_obj_set_style_pad_left(
        container,
        20,
        0
    );

    lv_obj_set_style_pad_right(
        container,
        20,
        0
    );

    lv_obj_add_flag(
        container,
        LV_OBJ_FLAG_SCROLLABLE
    );

    lv_obj_set_scroll_dir(
        container,
        LV_DIR_HOR
    );

    lv_obj_set_scroll_snap_x(
        container,
        LV_SCROLL_SNAP_NONE
    );

    lv_obj_set_scrollbar_mode(
        container,
        LV_SCROLLBAR_MODE_AUTO
    );

    lv_obj_clear_flag(
        container,
        LV_OBJ_FLAG_SCROLL_ELASTIC
    );


    // -------------------------------------------------
    // Items
    // -------------------------------------------------

    home = createItem(
        LV_SYMBOL_HOME,
        "Home",
        ScreenType::Home
    );

    weather = createItem(
        LV_SYMBOL_REFRESH,
        "Weather",
        ScreenType::Weather
    );
    
    casa = createItem(
        LV_SYMBOL_IMAGE,
        "Casa",
        ScreenType::Casa
    );

    docker = createItem(
        LV_SYMBOL_DIRECTORY,
        "Docker",
        ScreenType::Docker
    );

    devices = createItem(
        LV_SYMBOL_DRIVE,
        "Devices",
        ScreenType::Devices
    );

    config = createItem(
        LV_SYMBOL_EDIT,
        "Config",
        ScreenType::Config
    );

    system = createItem(
        LV_SYMBOL_LIST,
        "System",
        ScreenType::System
    );

    red = createItem(
        LV_SYMBOL_SHUFFLE,
        "Red",
        ScreenType::Red
    );

    grafana = createItem(
        LV_SYMBOL_EYE_OPEN,
        "Grafana",
        ScreenType::Grafana
    );

    settings = createItem(
        LV_SYMBOL_WIFI,
        "Settings",
        ScreenType::Settings
    );

    select(home);
}

void BottomMenu::layout()
{
    lv_obj_set_size(
        root,
        800,
        HEIGHT
    );

    lv_obj_align(
        root,
        LV_ALIGN_BOTTOM_MID,
        0,
        -35
    );

    lv_obj_set_size(
        container,
        800,
        HEIGHT
    );

    lv_obj_center(container);
}

lv_obj_t* BottomMenu::createItem(
    const char* icon,
    const char* text,
    ScreenType screen)
{
    lv_obj_t* panel = lv_obj_create(container);

    lv_obj_remove_style_all(panel);

    lv_obj_set_size(
        panel,
        120,
        100
    );

    lv_obj_set_flex_flow(
        panel,
        LV_FLEX_FLOW_COLUMN
    );

    lv_obj_set_flex_align(
        panel,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );

    lv_obj_clear_flag(
        panel,
        LV_OBJ_FLAG_SCROLLABLE
    );

    lv_obj_set_style_radius(
        panel,
        14,
        0
    );

    lv_obj_set_style_border_width(
        panel,
        0,
        0
    );

    lv_obj_set_style_bg_color(
        panel,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_opa(
        panel,
        LV_OPA_COVER,
        LV_PART_MAIN
    );


    // -------------------------------------------------
    // Icon
    // -------------------------------------------------

    lv_obj_t* iconLabel = lv_label_create(panel);

    lv_label_set_text(
        iconLabel,
        icon
    );

    lv_obj_set_style_text_color(
        iconLabel,
        Theme::Text(),
        0
    );

    lv_obj_set_style_text_font(
        iconLabel,
        &montserrat_28_es_26,
        0
    );


    // -------------------------------------------------
    // Text
    // -------------------------------------------------

    lv_obj_t* label = lv_label_create(panel);

    lv_label_set_text(
        label,
        text
    );

    lv_obj_set_style_text_color(
        label,
        Theme::Text(),
        0
    );


    // -------------------------------------------------
    // Event
    // -------------------------------------------------

    ItemData* data = new ItemData();

    data->menu = this;
    data->screen = screen;

    lv_obj_add_event_cb(
        panel,
        clickEvent,
        LV_EVENT_CLICKED,
        data
    );

    return panel;
}

void BottomMenu::select(ScreenType screen)
{
    switch(screen)
    {
        case ScreenType::Home:
            select(home);
            break;

        case ScreenType::Weather:
            select(weather);
            break;

        case ScreenType::Docker:
            select(docker);
            break;

        case ScreenType::Devices:
            select(devices);
            break;

        case ScreenType::Config:
            select(config);
            break;

        case ScreenType::Casa:
            select(casa);
            break;

        case ScreenType::System:
            select(system);
            break;

        case ScreenType::Settings:
            select(settings);
            break;

        case ScreenType::Red:
            select(red);
            break;

        case ScreenType::Grafana:
            select(grafana);
            break;

        default:
            break;
    }
}


void BottomMenu::select(lv_obj_t* item)
{
    if (!item)
        return;

    // -------------------------
    // Reset all items
    // -------------------------

    lv_obj_set_style_bg_color(
        home,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        weather,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        docker,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        devices,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        config,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        casa,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        system,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        settings,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        red,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        grafana,
        Theme::Panel(),
        LV_PART_MAIN
    );


    // -------------------------
    // Reset position
    // -------------------------

    lv_obj_set_style_translate_y(home, 0, LV_PART_MAIN);
    lv_obj_set_style_translate_y(weather, 0, LV_PART_MAIN);
    lv_obj_set_style_translate_y(docker, 0, LV_PART_MAIN);
    lv_obj_set_style_translate_y(devices, 0, LV_PART_MAIN);
    lv_obj_set_style_translate_y(config, 0, LV_PART_MAIN);
    lv_obj_set_style_translate_y(casa, 0, LV_PART_MAIN);
    lv_obj_set_style_translate_y(system, 0, LV_PART_MAIN);
    lv_obj_set_style_translate_y(settings, 0, LV_PART_MAIN);
    lv_obj_set_style_translate_y(red, 0, LV_PART_MAIN);
    lv_obj_set_style_translate_y(grafana, 0, LV_PART_MAIN);


    // -------------------------
    // Selected item
    // -------------------------

    lv_obj_set_style_bg_color(
        item,
        Theme::Primary(),
        LV_PART_MAIN
    );

    lv_obj_set_style_translate_y(
        item,
        -8,
        LV_PART_MAIN
    );

    // Si el cambio viene de un swipe, el item puede estar fuera de la pantalla:
    // desplaza el menu para que quede visible.
    lv_obj_scroll_to_view(item, LV_ANIM_ON);
}

void BottomMenu::clickEvent(lv_event_t* e)
{
    ItemData* data =
        (ItemData*)lv_event_get_user_data(e);

    if(data == nullptr)
        return;

    data->menu->onItemPressed(
        (lv_obj_t*)lv_event_get_target(e),
        data->screen);
}

void BottomMenu::onItemPressed(
        lv_obj_t* item,
        ScreenType screen)
{
    MainLayout::setBody(screen);
}

void BottomMenu::update()
{
    
}