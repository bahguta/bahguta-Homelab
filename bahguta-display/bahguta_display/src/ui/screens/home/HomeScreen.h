#pragma once

#include "../Screen.h"
#include <lvgl.h>
#include "../../widgets/cards/WeatherCard.h"
#include "../../widgets/cards/DockerCard.h"
#include "../../widgets/cards/DeviceCard.h"
#include "../../widgets/cards/SystemCard.h"
#include "../../widgets/cards/CasaCard.h"
#include "../../widgets/cards/SettingsCard.h"
#include "../../models/HomeInfo.h"
#include "../../widgets/cards/ConfigCard.h"
#include "../../widgets/cards/RedCard.h"
#include "../../widgets/cards/GrafanaCard.h"

class HomeScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void destroy() override;
    void show() override;
    void hide() override;

    void update();

private:

    void layoutContent();
    void createScreen(lv_obj_t* parent);
    void setWeather(HomeInfo home);

    lv_obj_t *screen;

    WeatherCard *weatherCard;
    DockerCard *dockerCard;
    DeviceCard *devicesCard;
    SystemCard *systemCard;
    SettingsCard *settingsCard;
    CasaCard *casaCard;
    ConfigCard *configCard;
    RedCard *redCard;
    GrafanaCard *grafanaCard;

};