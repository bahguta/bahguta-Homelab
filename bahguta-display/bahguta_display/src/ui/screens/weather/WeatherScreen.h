#pragma once

#include "../Screen.h"
#include "../../widgets/cards/WeatherCard.h"
#include "../../models/WeatherInfo.h"
#include <lvgl.h>

/*
 * TIEMPO: la misma card del Home (mismo ancho y alto, para que al cambiar de
 * pantalla no se note) y debajo 5 filas, una por dia:
 *
 *   Jueves  | 08:00      (icono) | 14:00      (icono) | 20:00      (icono)
 *   08/10   | 13° / 18°          | 20° / 26°          | 18° / 22°
 */
class WeatherScreen : public Screen
{
public:

    void create(lv_obj_t* parent) override;
    void show() override;
    void hide() override;
    void destroy() override;

private:

    static constexpr int FORECAST_ROWS = 5;

    void createContent();
    void createForecastRows();
    void update();

    // Tamano real de la pantalla (igual que en el Home): el body tiene padding, asi
    // que no vale suponer 800 x 970
    int bodyWidthPx() const;
    int bodyHeightPx() const;

    int rowHeight = 94;

    lv_obj_t* screen;

    WeatherCard* weatherCard = nullptr;

    lv_obj_t* forecastRow[FORECAST_ROWS] = { nullptr };
    lv_obj_t* forecastDayLabel[FORECAST_ROWS] = { nullptr };
    lv_obj_t* forecastEmpty[FORECAST_ROWS] = { nullptr };

    // Tres franjas por dia: hora, minima / maxima e icono (a la derecha del texto)
    lv_obj_t* hourLabel[FORECAST_ROWS][WeatherInfo::FORECAST_HOURS] = {};
    lv_obj_t* hourTemp[FORECAST_ROWS][WeatherInfo::FORECAST_HOURS] = {};
    lv_obj_t* hourIcon[FORECAST_ROWS][WeatherInfo::FORECAST_HOURS] = {};
};
