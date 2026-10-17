
#pragma once

#include <lvgl.h>
#include "../themes/Theme.h"
#include "../widgets/header/Header.h"
#include "../widgets/footer/Footer.h"
#include "../widgets/bottomMenu/BottomMenu.h"
#include "../screens/splash/SplashScreen.h"
#include "../screens/home/HomeScreen.h"
#include "../screens/weather/WeatherScreen.h"
#include "../screens/config/ConfigScreen.h"
#include "../screens/devices/DevicesScreen.h"
#include "../screens/docker/DockerScreen.h"
#include "../screens/casa/CasaScreen.h"
#include "../screens/system/SystemScreen.h"
#include "../screens/settings/SettingsScreen.h"
#include "../screens/red/RedScreen.h"
#include "../screens/grafana/GrafanaScreen.h"


class MainLayout
{
public:

    static void init();
    static void update();

    // Desde eventos de LVGL (sin mutex): toma el mutex de datos y cambia de pantalla
    static void setBody(ScreenType screenType);

    // Igual, pero con el mutex de datos YA tomado (p. ej. desde update())
    static void setBodyLocked(ScreenType screenType);
    static void setWifi(bool isConnected);
  
private:

    static void createRoot();
    static void createBody();
    static void bodyGestureEvent(lv_event_t* e);   // swipe entre pantallas
    static void createHeader();
    static void createBottomMenu();
    static void createFooter();
    static void createScreens();
    static void splashInit();

    static lv_obj_t* root;
    static lv_obj_t* body;

    static Header* header;
    static Footer* footer;
    static BottomMenu* bottomMenu;

    static SplashScreen* splash;
    static HomeScreen* home;
    static ConfigScreen* config;
    static DevicesScreen* devices;
    static DockerScreen* docker;
    static WeatherScreen* weather;
    static CasaScreen* casa;
    static SystemScreen* system;
    static SettingsScreen* settings;
    static RedScreen* red;
    static GrafanaScreen* grafana;

    static ScreenType currentScreen;
    static uint32_t lastHeavyUpdate;
    static constexpr uint32_t HEAVY_UPDATE_INTERVAL = 15000; // 15s
    
    static bool initialized;
};