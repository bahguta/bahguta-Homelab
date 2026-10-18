#include "SplashScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../ScreenType.h"
#include "../../services/ServiceManager.h"

#include <math.h>

static const uint32_t COLOR_GREEN = 0x6DDC36;

// ---------------------------------------------------------------
// Callbacks genericos de animacion
// ---------------------------------------------------------------
void SplashScreen::opaExec(void* var, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t*)var, (lv_opa_t)v, 0);
}

void SplashScreen::transYExec(void* var, int32_t v)
{
    lv_obj_set_style_translate_y((lv_obj_t*)var, v, 0);
}

void SplashScreen::breathExec(void* var, int32_t v)
{
    lv_obj_set_style_text_opa((lv_obj_t*)var, (lv_opa_t)v, 0);
}

void SplashScreen::progressExec(void* var, int32_t v)
{
    SplashScreen* self = (SplashScreen*)var;
    self->shown = v;
    lv_bar_set_value(self->progress, v, LV_ANIM_OFF);
    lv_label_set_text_fmt(self->percent, "%d%%", (int)v);
}

// Entrada: fade-in + pequeno desplazamiento hacia arriba (escalonado)
void SplashScreen::enter(lv_obj_t* o, uint32_t delay, int32_t dy)
{
    lv_obj_set_style_opa(o, LV_OPA_TRANSP, 0);

    lv_anim_t a;

    lv_anim_init(&a);
    lv_anim_set_var(&a, o);
    lv_anim_set_exec_cb(&a, opaExec);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_duration(&a, 600);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_var(&a, o);
    lv_anim_set_exec_cb(&a, transYExec);
    lv_anim_set_values(&a, dy, 0);
    lv_anim_set_duration(&a, 600);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}


// ---------------------------------------------------------------
// ICONO CASA + WIFI
// ---------------------------------------------------------------
static const lv_point_precise_t HOUSE_PATH[6] = {
    {10, 150}, {10, 80}, {90, 15}, {170, 80}, {170, 150}, {10, 150}
};

// v: 0..1000 -> porcentaje del recorrido del trazo de la casa
void SplashScreen::houseExec(void* var, int32_t v)
{
    SplashScreen* self = (SplashScreen*)var;

    int32_t remaining = (int32_t)((int64_t)self->houseTotal * v / 1000);
    int n = 0;

    self->housePts[n++] = HOUSE_PATH[0];

    for(int i = 0; i < 5; i++)
    {
        int32_t len = self->houseSegLen[i];

        if(remaining >= len)
        {
            self->housePts[n++] = HOUSE_PATH[i + 1];
            remaining -= len;
        }
        else
        {
            int32_t dx = HOUSE_PATH[i + 1].x - HOUSE_PATH[i].x;
            int32_t dy = HOUSE_PATH[i + 1].y - HOUSE_PATH[i].y;
            self->housePts[n].x = HOUSE_PATH[i].x + dx * remaining / len;
            self->housePts[n].y = HOUSE_PATH[i].y + dy * remaining / len;
            n++;
            break;
        }
    }

    lv_line_set_points(self->houseLine, self->housePts, n);
}

void SplashScreen::createHouseIcon()
{
    houseBox = lv_obj_create(screen);
    lv_obj_remove_style_all(houseBox);
    lv_obj_set_size(houseBox, 180, 160);
    lv_obj_align(houseBox, LV_ALIGN_CENTER, 0, -290);

    // Longitudes de cada tramo
    houseTotal = 0;
    for(int i = 0; i < 5; i++)
    {
        int32_t dx = HOUSE_PATH[i + 1].x - HOUSE_PATH[i].x;
        int32_t dy = HOUSE_PATH[i + 1].y - HOUSE_PATH[i].y;
        houseSegLen[i] = (int32_t)sqrtf((float)(dx * dx + dy * dy));
        houseTotal += houseSegLen[i];
    }

    // Casa
    houseLine = lv_line_create(houseBox);
    lv_obj_set_pos(houseLine, 0, 0);
    lv_obj_set_style_line_width(houseLine, 6, 0);
    lv_obj_set_style_line_color(houseLine, lv_color_hex(COLOR_GREEN), 0);
    lv_obj_set_style_line_rounded(houseLine, true, 0);
    houseExec(this, 0);

    // Wifi dentro de la casa: punto + 3 arcos
    const int32_t cx = 90, cy = 120;
    const int32_t radii[3] = { 16, 32, 48 };

    wifiDot = lv_obj_create(houseBox);
    lv_obj_remove_style_all(wifiDot);
    lv_obj_set_size(wifiDot, 12, 12);
    lv_obj_set_pos(wifiDot, cx - 6, cy - 6);
    lv_obj_set_style_radius(wifiDot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(wifiDot, lv_color_hex(COLOR_GREEN), 0);
    lv_obj_set_style_bg_opa(wifiDot, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(wifiDot, LV_OPA_TRANSP, 0);

    for(int i = 0; i < 3; i++)
    {
        lv_obj_t* a = lv_arc_create(houseBox);
        wifiArc[i] = a;

        lv_obj_remove_flag(a, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_size(a, radii[i] * 2, radii[i] * 2);
        lv_obj_set_pos(a, cx - radii[i], cy - radii[i]);
        lv_arc_set_bg_angles(a, 225, 315);

        lv_obj_set_style_arc_width(a, 6, LV_PART_MAIN);
        lv_obj_set_style_arc_color(a, lv_color_hex(COLOR_GREEN), LV_PART_MAIN);
        lv_obj_set_style_arc_rounded(a, true, LV_PART_MAIN);
        lv_obj_set_style_arc_opa(a, LV_OPA_TRANSP, LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(a, LV_OPA_TRANSP, LV_PART_KNOB);
        lv_obj_set_style_pad_all(a, 0, LV_PART_KNOB);
        lv_obj_set_style_opa(a, LV_OPA_TRANSP, 0);
    }
}

// ---------------------------------------------------------------
void SplashScreen::create(lv_obj_t* root)
{
    screen = lv_obj_create(nullptr);

    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, 800, 1280);
    lv_obj_set_style_bg_color(screen, Theme::Background(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    // ICONO CASA + WIFI
    createHouseIcon();

    // LOGO
    logo = lv_label_create(screen);
    lv_label_set_recolor(logo, true);
    lv_label_set_text(logo, "#6DDC36 bahguta# #FFFFFF Homelab#\n\n");
    lv_obj_set_style_text_font(logo, &montserrat_28_es_40, 0);
    lv_obj_align(logo, LV_ALIGN_CENTER, 0, -120);

    // ESTADO
    status = lv_label_create(screen);
    lv_label_set_text(status, "Inicializando...");
    lv_obj_align(status, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_text_color(status, Theme::Text(), 0);

    // PROGRESO (fino, redondeado, verde)
    progress = lv_bar_create(screen);
    lv_obj_set_size(progress, 420, 8);
    lv_obj_align(progress, LV_ALIGN_CENTER, 0, 100);
    lv_bar_set_range(progress, 0, 100);
    lv_bar_set_value(progress, 0, LV_ANIM_OFF);

    lv_obj_set_style_radius(progress, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(progress, lv_color_hex(0x2A2A2A), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(progress, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_set_style_radius(progress, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(progress, lv_color_hex(COLOR_GREEN), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(progress, lv_color_hex(0xB6F08A), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(progress, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(progress, LV_OPA_COVER, LV_PART_INDICATOR);

    // PORCENTAJE
    percent = lv_label_create(screen);
    lv_label_set_text(percent, "0%");
    lv_obj_align(percent, LV_ALIGN_CENTER, 0, 135);
    lv_obj_set_style_text_color(percent, lv_color_hex(COLOR_GREEN), 0);

    // FOOTER (igual que el de Home: barra 800x35, developer centro, version derecha)
    footerBar = lv_obj_create(screen);
    lv_obj_remove_style_all(footerBar);
    lv_obj_remove_flag(footerBar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(footerBar, 800, 35);
    lv_obj_align(footerBar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(footerBar, Theme::Header(), 0);
    lv_obj_set_style_bg_opa(footerBar, LV_OPA_COVER, 0);

    logoDeveloper = lv_label_create(footerBar);
    lv_label_set_text(logoDeveloper, "Developed by Plamen");
    lv_obj_set_style_text_color(logoDeveloper, lv_color_hex(COLOR_GREEN), 0);
    lv_obj_align(logoDeveloper, LV_ALIGN_CENTER, 0, 0);

    version = lv_label_create(footerBar);
    lv_label_set_text(version, "v--");
    lv_obj_set_style_text_color(version, Theme::Text(), 0);
    lv_obj_align(version, LV_ALIGN_RIGHT_MID, -20, 0);

    refreshVersion();
}

// Version del proyecto: ultima guardada en NVS o la que acaba de informar el backend
void SplashScreen::refreshVersion()
{
    if (!version) return;

    char v[16];

    ServiceManager::lockData();
    strlcpy(v, ServiceManager::api().server().version, sizeof(v));
    ServiceManager::unlockData();

    if (!v[0]) return;

    char buffer[24];
    snprintf(buffer, sizeof(buffer), "v%s", v);
    lv_label_set_text(version, buffer);
}

void SplashScreen::setProgress(int value)
{
    if(!progress) return;

    lv_anim_del(this, progressExec);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, this);
    lv_anim_set_exec_cb(&a, progressExec);
    lv_anim_set_values(&a, shown, value);
    lv_anim_set_duration(&a, 550);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

void SplashScreen::setStatus(const char *text)
{
    // OJO: antes aqui se llamaba lv_timer_handler() desde dentro de un
    // timer (reentrancia) -> posible causa de tirones. Eliminado.
    lv_label_set_text(status, text);
}

void SplashScreen::show()
{
    lv_screen_load(screen);
    play();
}

void SplashScreen::hide()
{
}

void SplashScreen::destroy()
{
    if(screen)
    {
        lv_obj_delete(screen);
        screen = nullptr;
    }
}

void SplashScreen::play()
{
    // show() y splashInit() llamaban a play(): doble timer y doble fade.
    if(started) return;
    started = true;

    step = 0;

    // 1) La casa se dibuja (trazo continuo)
    lv_anim_t h;
    lv_anim_init(&h);
    lv_anim_set_var(&h, this);
    lv_anim_set_exec_cb(&h, houseExec);
    lv_anim_set_values(&h, 0, 1000);
    lv_anim_set_duration(&h, 900);
    lv_anim_set_path_cb(&h, lv_anim_path_ease_in_out);
    lv_anim_start(&h);

    // 2) Wifi: punto y arcos de dentro hacia fuera
    lv_anim_t w;
    lv_obj_t* wifi[4] = { wifiDot, wifiArc[0], wifiArc[1], wifiArc[2] };
    for(int i = 0; i < 4; i++)
    {
        lv_anim_init(&w);
        lv_anim_set_var(&w, wifi[i]);
        lv_anim_set_exec_cb(&w, opaExec);
        lv_anim_set_values(&w, 0, 255);
        lv_anim_set_duration(&w, 350);
        lv_anim_set_delay(&w, 900 + i * 150);
        lv_anim_set_path_cb(&w, lv_anim_path_ease_out);
        lv_anim_start(&w);
    }

    // 3) Resto de la pantalla, escalonado
    enter(logo,          700,   30);
    enter(status,        900,   20);
    enter(progress,     1000,   20);
    enter(percent,      1100,   20);
    enter(footerBar,    1200,    0);

    // Logo "respirando" cuando termina la entrada
    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, logo);
    lv_anim_set_exec_cb(&b, breathExec);
    lv_anim_set_values(&b, 255, 140);
    lv_anim_set_duration(&b, 1400);
    lv_anim_set_playback_duration(&b, 1400);
    lv_anim_set_repeat_count(&b, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_delay(&b, 1500);
    lv_anim_set_path_cb(&b, lv_anim_path_ease_in_out);
    lv_anim_start(&b);

    lv_timer_create(animationTimer, 580, this);
}

void SplashScreen::animationTimer(lv_timer_t *timer)
{
    SplashScreen* self = (SplashScreen*)lv_timer_get_user_data(timer);

    self->nextStep();
    self->refreshVersion();

    if(self->finished())
        lv_timer_delete(timer);
}

// step 0-5: tramos de carga; step 6: pausa para que la barra llegue al 100%
bool SplashScreen::finished() const
{
    return step >= 7;
}

void SplashScreen::nextStep()
{
    switch(step)
    {
        case 0: setStatus("Inicializando display...");   setProgress(15);  break;
        case 1: setStatus("Inicializando touch...");     setProgress(30);  break;
        case 2: setStatus("Inicializando servicios..."); setProgress(45);  break;
        case 3: setStatus("Conectando WiFi...");         setProgress(65);  break;
        case 4: setStatus("Conectando Backend...");      setProgress(85);  break;
        case 5: setStatus("Cargando Dashboard...");      setProgress(100); break;
        default: break;
    }

    step++;
}

// ---------------------------------------------------------------
// SALIDA
// ---------------------------------------------------------------
void SplashScreen::setContentOpa(lv_opa_t opa)
{
    lv_obj_t* items[] = { houseBox, logo, status, progress, percent, footerBar };

    for(lv_obj_t* o : items)
        if(o) lv_obj_set_style_opa(o, opa, 0);
}

void SplashScreen::exitExec(void* var, int32_t v)
{
    ((SplashScreen*)var)->setContentOpa((lv_opa_t)v);
}

void SplashScreen::exitReady(lv_anim_t* a)
{
    SplashScreen* self = (SplashScreen*)a->var;

    // auto_del = true: LVGL borra la pantalla del splash al terminar.
    lv_screen_load_anim(self->nextScreen,
                        LV_SCREEN_LOAD_ANIM_FADE_IN,
                        350, 0, true);

    self->screen = nullptr;
}

void SplashScreen::exit(lv_obj_t* next)
{
    if(exiting || !screen) return;
    exiting    = true;
    nextScreen = next;

    lv_anim_del(logo, breathExec);
    lv_obj_set_style_text_opa(logo, LV_OPA_COVER, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, this);
    lv_anim_set_exec_cb(&a, exitExec);
    lv_anim_set_values(&a, 255, 0);
    lv_anim_set_duration(&a, 350);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_completed_cb(&a, exitReady);
    lv_anim_start(&a);
}
