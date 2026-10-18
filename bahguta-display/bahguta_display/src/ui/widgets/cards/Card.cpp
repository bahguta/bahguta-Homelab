#include "Card.h"

#include "../../themes/Theme.h"
#include "../../layout/MainLayout.h"
#include "../../screens/ScreenType.h"

Card::Card(lv_obj_t* parent)
{
    card = lv_obj_create(parent);

    root = card;


    // -------------------------------------------------
    // Appearance
    // -------------------------------------------------

    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_text_color(
        root,
        Theme::Text(),
        0
    );

    lv_obj_set_style_bg_color(
        root,
        Theme::Panel(),
        0
    );

    lv_obj_set_style_border_width(
        root,
        0,
        0
    );

    lv_obj_set_style_radius(
        root,
        16,
        0
    );


    // -------------------------------------------------
    // Interaction
    // -------------------------------------------------

    lv_obj_add_flag(
        card,
        LV_OBJ_FLAG_CLICKABLE
    );

    // NOTA: el registro de LV_EVENT_CLICKED se hace en setClickScreen(),
    // que cada subclase llama con un ClickData* real. Registrarlo también
    // aquí (con "this" en vez de un ClickData*) provocaba que clickEvent()
    // leyera memoria ajena (la vtable como Card*, y "root" como ScreenType),
    // llamando a MainLayout::setBody() con un ScreenType basura en CADA
    // click, antes de que el callback correcto lo corrigiera.

    lv_obj_set_style_bg_color(
        card,
        Theme::Panel(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_color(
        card,
        Theme::Primary(),
        LV_PART_MAIN | LV_STATE_PRESSED
    );

    // La card puede recibir clicks
    lv_obj_add_flag(
        card,
        LV_OBJ_FLAG_CLICKABLE
    );
}

// void Card::setScreen(ScreenType screen)
// {
//     targetScreen = screen;
// }

// void Card::clickEvent(lv_event_t* event)
// {
//     if (!event)
//         return;

//     Card* card =
//         static_cast<Card*>(
//             lv_event_get_user_data(event)
//         );

//     if (!card)
//         return;

//     card->onClicked();
// }

// void Card::onClicked()
// {
//     MainLayout::setBody(targetScreen);
// }

void Card::setClickScreen(ScreenType screen)
{
    ClickData* data = new ClickData();

    data->card = this;
    data->screen = screen;
    data->pressPoint = {0, 0};
    data->pressed = false;

    // PRESSED guarda el punto inicial; CLICKED solo navega si el dedo casi no se movio.
    lv_obj_add_event_cb(
        card,
        pressEvent,
        LV_EVENT_PRESSED,
        data
    );

    lv_obj_add_event_cb(
        card,
        clickEvent,
        LV_EVENT_CLICKED,
        data
    );
}

// Si el dedo se mueve mas que esto entre pulsar y soltar, es un swipe, no un tap.
static constexpr int TAP_MAX_MOVE = 20;

void Card::pressEvent(lv_event_t* e)
{
    ClickData* data =
        static_cast<ClickData*>(
            lv_event_get_user_data(e)
        );

    lv_indev_t* indev = lv_event_get_indev(e);

    if (!data || !indev)
        return;

    lv_indev_get_point(indev, &data->pressPoint);
    data->pressed = true;
}

void Card::clickEvent(lv_event_t* e)
{
    ClickData* data =
        static_cast<ClickData*>(
            lv_event_get_user_data(e)
        );

    if (!data)
        return;

    // Descarta el click si en realidad fue un swipe que empezo sobre la card
    lv_indev_t* indev = lv_event_get_indev(e);

    if (indev && data->pressed)
    {
        lv_point_t now;
        lv_indev_get_point(indev, &now);

        const int dx = now.x - data->pressPoint.x;
        const int dy = now.y - data->pressPoint.y;

        data->pressed = false;

        if (dx > TAP_MAX_MOVE || dx < -TAP_MAX_MOVE ||
            dy > TAP_MAX_MOVE || dy < -TAP_MAX_MOVE)
            return;
    }

    data->card->onClick(data->screen);
}

void Card::onClick(ScreenType screen)
{
    MainLayout::setBody(screen);
}