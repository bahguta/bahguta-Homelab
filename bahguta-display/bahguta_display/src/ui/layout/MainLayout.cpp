#include "MainLayout.h"
#include "../services/ServiceManager.h"

SplashScreen* MainLayout::splash = nullptr;
uint32_t MainLayout::lastHeavyUpdate = 0;

HomeScreen* MainLayout::home = nullptr;
ConfigScreen* MainLayout::config = nullptr;
DevicesScreen* MainLayout::devices = nullptr;
DockerScreen* MainLayout::docker = nullptr;
WeatherScreen* MainLayout::weather = nullptr;
CasaScreen* MainLayout::casa = nullptr;
SystemScreen* MainLayout::system = nullptr;
SettingsScreen* MainLayout::settings = nullptr;
RedScreen* MainLayout::red = nullptr;
GrafanaScreen* MainLayout::grafana = nullptr;

ScreenType MainLayout::currentScreen = ScreenType::Splash;


lv_obj_t* MainLayout::root = nullptr;
lv_obj_t* MainLayout::body = nullptr;

Header* MainLayout::header = nullptr;
Footer* MainLayout::footer = nullptr;
BottomMenu* MainLayout::bottomMenu = nullptr;

bool MainLayout::initialized = false;


void MainLayout::init() {

    createRoot();
    createHeader();
    createFooter();
    createBottomMenu();
    createBody();

    createScreens();
    splashInit();

    
}


void MainLayout::createRoot() {
    root = lv_obj_create(nullptr);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root,800,1280);
    lv_obj_set_style_bg_color(root,Theme::Background(),0);
    lv_obj_set_style_bg_opa(root,LV_OPA_COVER,0);
}

void MainLayout::createBody(){
    body = lv_obj_create(root);
    // El menu inferior empieza en y = 1115 (1280 - 35 de pie - 130 de menu) y el body
    // empezaba en 90 con 970 de alto (terminaba en 1060): quedaban 55 px muertos.
    // Ahora termina en 1110, a 5 px del menu. Las pantallas miden su area con
    // lv_obj_get_height(screen) en vez de suponer un alto fijo.
    lv_obj_set_size(body,800,1020);
    lv_obj_set_pos(body,0,90);
    lv_obj_set_style_radius(body,0,0);
    lv_obj_set_style_border_width(body,0,0);
    lv_obj_set_style_text_color(body,Theme::Text(),0);
    lv_obj_set_style_bg_color(body,Theme::Panel(),0);

    lv_obj_set_style_bg_opa(body, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(body,0,0);
    lv_obj_set_style_shadow_width(body,0,0);
    lv_obj_set_style_radius(body,0,0);

    // Swipe horizontal: cambia de pantalla en el orden del menu inferior.
    // LVGL envia LV_EVENT_GESTURE al primer ancestro SIN el flag GESTURE_BUBBLE.
    // Todos los objetos hijos lo tienen por defecto (incluido body), asi que el
    // evento sube hasta la pantalla raiz (root, que no tiene padre ni flag).
    // Por eso el handler va en root. Tambien se registra en body por si algun
    // dia se le quita el flag: solo uno de los dos recibe cada gesto.
    lv_obj_clear_flag(body, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(root, bodyGestureEvent, LV_EVENT_GESTURE, nullptr);
    lv_obj_add_event_cb(body, bodyGestureEvent, LV_EVENT_GESTURE, nullptr);
}

// Mismo orden que los items del BottomMenu
static const ScreenType SWIPE_ORDER[] = {
    ScreenType::Home,
    ScreenType::Weather,
    ScreenType::Casa,
    ScreenType::Docker,
    ScreenType::Devices,
    ScreenType::Config,
    ScreenType::System,
    ScreenType::Red,
    ScreenType::Grafana,
    ScreenType::Settings,
};
static constexpr int SWIPE_COUNT = sizeof(SWIPE_ORDER) / sizeof(SWIPE_ORDER[0]);

void MainLayout::bodyGestureEvent(lv_event_t* e)
{
    lv_indev_t* indev = lv_indev_active();
    if (!indev)
        return;

    const lv_dir_t dir = lv_indev_get_gesture_dir(indev);
    if (dir != LV_DIR_LEFT && dir != LV_DIR_RIGHT)
        return;

    int idx = -1;
    for (int i = 0; i < SWIPE_COUNT; i++)
    {
        if (SWIPE_ORDER[i] == currentScreen)
        {
            idx = i;
            break;
        }
    }

    if (idx < 0)
        return;     // Splash u otra pantalla fuera del menu

    // Dedo hacia la izquierda -> pantalla siguiente; hacia la derecha -> anterior
    const int next = (dir == LV_DIR_LEFT) ? idx + 1 : idx - 1;
    if (next < 0 || next >= SWIPE_COUNT)
        return;     // sin vuelta: Home y Settings son los extremos

    lv_indev_wait_release(indev);   // un solo cambio por gesto
    setBody(SWIPE_ORDER[next]);
}

void MainLayout::createHeader() {
    header = new Header(root);
}

void MainLayout::createFooter() {
    footer = new Footer(root);
}

void MainLayout::createBottomMenu() {
    bottomMenu = new BottomMenu(root);
}

void MainLayout::createScreens() {
    splash = new SplashScreen();
    home = new HomeScreen();
    weather = new WeatherScreen();
    docker = new DockerScreen();
    devices = new DevicesScreen();
    config = new ConfigScreen();
    casa = new CasaScreen();
    system = new SystemScreen();
    settings = new SettingsScreen();
    red = new RedScreen();
    grafana = new GrafanaScreen();

    splash->create(root);
    splash->show();

    home->create(body);
    weather->create(body);
    docker->create(body);
    devices->create(body);
    config->create(body);
    casa->create(body);
    system->create(body);
    settings->create(body);
    red->create(body);
    grafana->create(body);

    home->hide();
    weather->hide();
    docker->hide();
    devices->hide();
    config->hide();
    casa->hide();
    system->hide();
    settings->hide();
    red->hide();
    grafana->hide();

    initialized = false;
}

void MainLayout::setBody(ScreenType screen)
{
    // Los show() de las pantallas leen datos que la task de red reescribe:
    // se ejecutan siempre con el mutex de datos tomado.
    ServiceManager::lockData();
    setBodyLocked(screen);
    ServiceManager::unlockData();
}

void MainLayout::setBodyLocked(ScreenType screen)
{
    currentScreen = screen;

    home->hide();
    weather->hide();
    docker->hide();
    devices->hide();
    config->hide();
    casa->hide();
    system->hide();
    settings->hide();
    red->hide();
    grafana->hide();

    switch(screen)
    {
        case ScreenType::Home:
            home->show();
            break;

        case ScreenType::Weather:
            weather->show();
            break;

        case ScreenType::Docker:
            docker->show();
            break;

        case ScreenType::Devices:
            devices->show();
            break;

        case ScreenType::Config:
            config->show();
            break;

        case ScreenType::Casa:
            casa->show();
            break;

        case ScreenType::System:
            system->show();
            break;
            
        case ScreenType::Settings:
            settings->show();
            break;

        case ScreenType::Red:
            red->show();
            break;

        case ScreenType::Grafana:
            grafana->show();
            break;

        default:
            break;
    }

    bottomMenu->select(screen);
}

void MainLayout::update()
{
    if(!initialized && splash && splash->finished())
    {
        initialized = true;
        splash->exit(root);  // fade-out + transicion a root; el splash se borra solo
        setBodyLocked(ScreenType::Home); // update() ya tiene el mutex
    }

    header->update();
    footer->update();
    bottomMenu->update();

    if (millis() - lastHeavyUpdate >= HEAVY_UPDATE_INTERVAL)
    {
        lastHeavyUpdate = millis();

        switch (currentScreen)
        {
            case ScreenType::Home:
                home->update();
                break;

            case ScreenType::Docker:
                docker->update();
                break;

            case ScreenType::Casa:
                casa->update();
                break;

            case ScreenType::Devices:
                devices->update();
                break;

            case ScreenType::System:
                system->update();
                break;

            case ScreenType::Settings:
                settings->refreshLive();
                break;

            case ScreenType::Red:
                red->update();
                break;

            case ScreenType::Grafana:
                grafana->update();
                break;

            default:
                break;
        }
    }

}





void MainLayout::splashInit(){
    splash->play();
}

void MainLayout::setWifi(bool isConnected){
    header->setWifi(isConnected);
}

    