#include "HomeScreen.h"
#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include <Arduino.h>
#include "../../models/HomeInfo.h"
#include "../../services/ServiceManager.h"


void HomeScreen::create(lv_obj_t* parent)
{
    createScreen(parent);
    layoutContent();
}

void HomeScreen::createScreen(lv_obj_t* parent)
{
    screen = lv_obj_create(parent);
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(screen, 0, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(screen,Theme::Background(),0);
    lv_obj_update_layout(screen);
}

void HomeScreen::show()
{
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_HIDDEN);

    weatherCard->resume();
    update();
}

void HomeScreen::hide()
{
    lv_obj_add_flag(screen, LV_OBJ_FLAG_HIDDEN);

    weatherCard->pause();
}

void HomeScreen::destroy()
{
    if(screen)
    {
        lv_obj_delete(screen);
        screen = nullptr;
    }
}

void HomeScreen::layoutContent()
{
    const int bodyWidth = lv_obj_get_width(screen);
    const int left = ContentLayout::LEFT;
    const int top  = ContentLayout::TOP;
    const int gap  = ContentLayout::GAP;
    const int width = ContentLayout::contentWidth(bodyWidth);

    //WeatherCard
    weatherCard = new WeatherCard(screen);
    lv_obj_set_pos(weatherCard->object(), left, top);
    lv_obj_set_size(weatherCard->object(),width,ContentLayout::WEATHER_HEIGHT);
    weatherCard->layout();
    //weatherCard->setScreen(ScreenType::Weather);

    const int cardTop = top + ContentLayout::WEATHER_HEIGHT + gap;
    const int cardWidth = ContentLayout::cardWidth(bodyWidth);

    //DockerCard
    dockerCard = new DockerCard(screen);
    lv_obj_set_pos(dockerCard->object(),left,cardTop + ContentLayout::CARD_HEIGHT + gap);
    lv_obj_set_size(dockerCard->object(),cardWidth,ContentLayout::CARD_HEIGHT);
    dockerCard->layout();
    //dockerCard->setScreen(ScreenType::Docker);

    //DevicesCard
    devicesCard = new DeviceCard(screen);
    // Fila 2, a la derecha de Docker
    lv_obj_set_pos(devicesCard->object(),left + cardWidth + gap,cardTop + ContentLayout::CARD_HEIGHT + gap);
    lv_obj_set_size(devicesCard->object(),cardWidth,ContentLayout::CARD_HEIGHT);
    devicesCard->layout();
    //devicesCard->setScreen(ScreenType::Devices);

    // Fila 1: System, Red, Grafana (3 columnas, igual que la fila 3)
    const int row1Width = ContentLayout::cardWidth(bodyWidth, 3);

    //SystemCard
    systemCard = new SystemCard(screen);
    lv_obj_set_pos(systemCard->object(),left,cardTop);
    lv_obj_set_size(systemCard->object(),row1Width,ContentLayout::CARD_HEIGHT);
    systemCard->setCardWidth(row1Width);
    systemCard->layout();

    // RedCard
    redCard = new RedCard(screen);
    lv_obj_set_pos(redCard->object(),left + row1Width + gap,cardTop);
    lv_obj_set_size(redCard->object(),row1Width,ContentLayout::CARD_HEIGHT);
    redCard->layout();

    // GrafanaCard
    grafanaCard = new GrafanaCard(screen);
    lv_obj_set_pos(grafanaCard->object(),left + (row1Width + gap) * 2,cardTop);
    lv_obj_set_size(grafanaCard->object(),row1Width,ContentLayout::CARD_HEIGHT);
    grafanaCard->layout();

    const int row3Top = cardTop + (ContentLayout::CARD_HEIGHT + gap) * 2;

    const int row3Width = ContentLayout::cardWidth(bodyWidth, 3);

    // CasaCard
    casaCard = new CasaCard(screen);
    lv_obj_set_pos(casaCard->object(),left,row3Top);
    lv_obj_set_size(casaCard->object(),row3Width,ContentLayout::CARD_HEIGHT);
    casaCard->layout();

    // SettingsCard
    settingsCard = new SettingsCard(screen);
    lv_obj_set_pos(settingsCard->object(),left + row3Width + gap,row3Top);
    lv_obj_set_size(settingsCard->object(),row3Width,ContentLayout::CARD_HEIGHT);
    settingsCard->layout();

    // ConfigCard
    configCard = new ConfigCard(screen);
    lv_obj_set_pos(configCard->object(),left + (row3Width + gap) * 2,row3Top);
    lv_obj_set_size(configCard->object(),row3Width,ContentLayout::CARD_HEIGHT);
    configCard->layout();

    


}

void HomeScreen::setWeather(HomeInfo home){
    if (home.weather.success)
    {
        // ================================
        // LOCATION
        // ================================

        weatherCard->setLocation(
            home.weather.location.c_str()
        );


        // ================================
        // TEMPERATURE
        // ================================

        char temperature[16];

        snprintf(
            temperature,
            sizeof(temperature),
            "%.1f °C",
            home.weather.temperature
        );

        weatherCard->setTemperature(
            temperature
        );


        // ================================
        // DESCRIPTION
        // ================================

        weatherCard->setDescription(
            home.weather.description.c_str()
        );


        // ================================
        // WEATHER ICON
        // ================================

        weatherCard->setIcon(
            home.weather.icon.c_str()
        );


        // ================================
        // HUMIDITY
        // ================================

        weatherCard->setHumidity(
            home.weather.humidity
        );


        // ================================
        // WIND
        // ================================

        weatherCard->setWind(
            home.weather.windSpeed
        );


        // ================================
        // PRESSURE
        // ================================

        weatherCard->setPressure(
            home.weather.pressure
        );


        // ================================
        // SUN
        // ================================

        weatherCard->setSunrise(
            home.weather.sunrise
        );

        weatherCard->setSunset(
            home.weather.sunset
        );
    }
}

void HomeScreen::update()
{
    const HomeInfo& home =
        ServiceManager::home().home();
        
    setWeather(home);

    // DOKER
    dockerCard->setRunning(
        home.docker.online,
        home.docker.total);

    // DEVICE
    devicesCard->setOnline(
        home.devices.online,
        home.devices.total);


    // SERVER
    systemCard->setServerCpu(
        home.server.cpu
    );
    systemCard->setServerRam(
        home.server.ram
    );
    systemCard->setServerUptime(
        home.server.uptime
    );


    // //TABLET
    // systemCard->setTabletCpu(
    //     home.system.tabletCpu
    // );

    systemCard->setTabletRam(
        home.system.ram
    );
    systemCard->setTabletUptime(
        home.system.uptime
    );




    // RED
    redCard->setClients(
        home.red.total,
        home.red.connected);

    // GRAFANA (disco principal en %, temperatura del servidor)
    double diskPercent = -1;
    if (home.grafana.storageTotalGb > 0 && home.grafana.storageUsedGb >= 0)
        diskPercent = home.grafana.storageUsedGb * 100.0 / home.grafana.storageTotalGb;
    else if (home.grafana.rootTotalGb > 0 && home.grafana.rootUsedGb >= 0)
        diskPercent = home.grafana.rootUsedGb * 100.0 / home.grafana.rootTotalGb;

    grafanaCard->setSummary(
        diskPercent,
        home.grafana.tempC);

    // CASA: temperatura media y numero de habitaciones (mismos datos que CasaScreen)
    const CasaInfo& casa = ServiceManager::api().casa();

    casaCard->setClimate(
        casa.averageTemperature,
        casa.temperatureAvailable,
        casa.averageHumidity,
        casa.humidityAvailable);

    casaCard->setRooms(casa.roomCount);

    //TODO:
    //SettingsCard *settingsCard;
}