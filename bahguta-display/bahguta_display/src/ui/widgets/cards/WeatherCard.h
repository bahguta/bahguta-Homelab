#pragma once

#include "Card.h"

class WeatherCard : public Card
{
public:

    WeatherCard(lv_obj_t* parent);
    ~WeatherCard();

    void layout() override;

    void setLocation(const char* txt);
    void setTemperature(const char* txt);
    void setDescription(const char* txt);

    void setIcon(const char* txt);
    void setHumidity(int value);
    void setWind(float value);
    void setPressure(int value);
    void setSunrise(const char* value);
    void setSunset(const char* value);

    void pause();
    void resume();

private:

    // ========================================================
    // WEATHER SCENE
    // ========================================================

    enum class WeatherScene
    {
        ClearDay,
        ClearNight,
        PartlyCloudyNight,
        CloudyNight,
        RainNight,
        StormNight,
        SnowNight,
        PartlyCloudy,
        Cloudy,
        Rain,
        Storm,
        Snow,
        Unknown
    };


    // ========================================================
    // MAIN WEATHER HUD
    // ========================================================

    lv_obj_t* icon = nullptr;

    lv_obj_t* location = nullptr;
    lv_obj_t* temperature = nullptr;
    lv_obj_t* description = nullptr;


    // ========================================================
    // BOTTOM INFORMATION
    // ========================================================

    lv_obj_t* bottomInfo = nullptr;


    // --------------------------------------------------------
    // HUMIDITY
    // --------------------------------------------------------

    lv_obj_t* humidityItem = nullptr;
    lv_obj_t* humidityIcon = nullptr;
    lv_obj_t* humidity = nullptr;


    // --------------------------------------------------------
    // WIND
    // --------------------------------------------------------

    lv_obj_t* windItem = nullptr;
    lv_obj_t* windIcon = nullptr;
    lv_obj_t* wind = nullptr;


    // --------------------------------------------------------
    // PRESSURE
    // --------------------------------------------------------

    lv_obj_t* pressureItem = nullptr;
    lv_obj_t* pressureIcon = nullptr;
    lv_obj_t* pressure = nullptr;


    // --------------------------------------------------------
    // SUNRISE
    // --------------------------------------------------------

    lv_obj_t* sunriseItem = nullptr;
    lv_obj_t* sunriseIcon = nullptr;
    lv_obj_t* sunrise = nullptr;


    // --------------------------------------------------------
    // SUNSET
    // --------------------------------------------------------

    lv_obj_t* sunsetItem = nullptr;
    lv_obj_t* sunsetIcon = nullptr;
    lv_obj_t* sunset = nullptr;


    WeatherScene scene = WeatherScene::Unknown;

    // ============================================================
    // FRAME ANIMATION STATE
    // ============================================================

    // Carpeta (dentro de LittleFS, /weather/<folder>/frame_NNN.png)
    // de la escena actualmente activa.
    char frameFolder[32] = "not-available";

    int frameCount = 1;
    int frameIndex = 0;

    lv_timer_t* animationTimer = nullptr;

    // ============================================================
    // CALLBACKS
    // ============================================================

    static void frameTimerCallback(lv_timer_t* timer);

    // ============================================================
    // SCENE / ANIMATION
    // ============================================================

    void updateSceneFromIcon(const char* iconCode);
    void configureScene(WeatherScene newScene);
    void updateFrameSrc();
};
