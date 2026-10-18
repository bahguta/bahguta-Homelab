#include "WeatherCard.h"
#include "../../../fonts/Fonts.h"
#include "../../themes/Theme.h"

#include <math.h>
#include <Arduino.h>
#include <string.h>
#include <lvgl.h>
#include <LittleFS.h>

/*
 * Icono principal. El objeto mide 128x128 (tamano del PNG) pero se DIBUJA
 * escalado alrededor de su centro (2.5x = 320x320), asi que desborda por todos
 * los lados 96 px. La posicion se calcula sobre el DIBUJO, no sobre el objeto:
 *
 *   ICON_MARGIN_RIGHT : distancia del borde derecho del dibujo al borde de la tarjeta
 *   ICON_TOP          : distancia del borde superior del dibujo al borde de la tarjeta
 *
 * Para moverlo: mas ICON_MARGIN_RIGHT = mas a la izquierda; mas ICON_TOP = mas abajo.
 */
static constexpr int ICON_SRC_SIZE     = 128;
static constexpr int ICON_SCALE        = 640;                                   // 256 = 100 %
static constexpr int ICON_DRAW_SIZE    = ICON_SRC_SIZE * ICON_SCALE / 256;      // 320
static constexpr int ICON_OVERFLOW     = (ICON_DRAW_SIZE - ICON_SRC_SIZE) / 2;  // 96
static constexpr int ICON_MARGIN_RIGHT = 40;
static constexpr int ICON_TOP          = 20;

// ============================================================
// CONSTRUCTOR
// ============================================================

WeatherCard::WeatherCard(
    lv_obj_t* parent
)
    : Card(parent)
{
    setClickScreen(
        ScreenType::Weather
    );


    // ========================================================
    // CARD STYLE
    // ========================================================

    lv_obj_set_style_radius(
        card,
        16,
        0
    );


    lv_obj_set_style_border_width(
        card,
        0,
        0
    );


    lv_obj_set_style_text_color(
        card,
        Theme::Text(),
        0
    );


    lv_obj_set_style_bg_color(
        card,
        Theme::Weather(),
        0
    );


    // ========================================================
    // MAIN WEATHER ICON
    // ========================================================

    icon =
        lv_image_create(card);

    /*
     * Los frames se generaron a 128x128 para que quepan
     * de sobra en LittleFS. Los escalamos 2.5x aqui en vez
     * de regenerarlos a mayor resolucion (256 = 100%).
     */

    lv_image_set_pivot(icon, ICON_SRC_SIZE / 2, ICON_SRC_SIZE / 2);   // centro, explicito
    lv_image_set_scale(
        icon,
        ICON_SCALE
    );


    // ========================================================
    // LOCATION
    // ========================================================

    location =
        lv_label_create(card);


    lv_obj_set_style_text_color(
        location,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_font(
        location,
        &montserrat_28_es_18,
        0
    );


    // ========================================================
    // TEMPERATURE
    // ========================================================

    temperature =
        lv_label_create(card);


    lv_obj_set_style_text_color(
        temperature,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_font(
        temperature,
        &montserrat_28_es_40,
        0
    );


    // ========================================================
    // DESCRIPTION
    // ========================================================

    description =
        lv_label_create(card);


    lv_obj_set_style_text_color(
        description,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_font(
        description,
        &montserrat_28_es_18,
        0
    );


    // ========================================================
    // BOTTOM INFORMATION CONTAINER
    // ========================================================

    bottomInfo =
        lv_obj_create(card);


    lv_obj_remove_style_all(
        bottomInfo
    );


    lv_obj_set_width(
        bottomInfo,
        lv_pct(100)
    );


    lv_obj_set_height(
        bottomInfo,
        40
    );


    lv_obj_set_style_pad_left(
        bottomInfo,
        20,
        0
    );


    lv_obj_set_style_pad_right(
        bottomInfo,
        20,
        0
    );


    lv_obj_set_style_pad_column(
        bottomInfo,
        0,
        0
    );


    lv_obj_set_flex_flow(
        bottomInfo,
        LV_FLEX_FLOW_ROW
    );


    lv_obj_set_flex_align(
        bottomInfo,
        LV_FLEX_ALIGN_SPACE_BETWEEN,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );


    // ========================================================
    // HUMIDITY ITEM
    // ========================================================

    humidityItem =
        lv_obj_create(bottomInfo);


    lv_obj_remove_style_all(
        humidityItem
    );


    lv_obj_set_height(
        humidityItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_width(
        humidityItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_flex_flow(
        humidityItem,
        LV_FLEX_FLOW_ROW
    );


    lv_obj_set_flex_align(
        humidityItem,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );


    humidityIcon =
        lv_label_create(
            humidityItem
        );


    humidity =
        lv_label_create(
            humidityItem
        );


    // ========================================================
    // WIND ITEM
    // ========================================================

    windItem =
        lv_obj_create(bottomInfo);


    lv_obj_remove_style_all(
        windItem
    );


    lv_obj_set_height(
        windItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_width(
        windItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_flex_flow(
        windItem,
        LV_FLEX_FLOW_ROW
    );


    lv_obj_set_flex_align(
        windItem,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );


    windIcon =
        lv_label_create(
            windItem
        );


    wind =
        lv_label_create(
            windItem
        );


    // ========================================================
    // PRESSURE ITEM
    // ========================================================

    pressureItem =
        lv_obj_create(bottomInfo);


    lv_obj_remove_style_all(
        pressureItem
    );


    lv_obj_set_height(
        pressureItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_width(
        pressureItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_flex_flow(
        pressureItem,
        LV_FLEX_FLOW_ROW
    );


    lv_obj_set_flex_align(
        pressureItem,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );


    pressureIcon =
        lv_label_create(
            pressureItem
        );


    pressure =
        lv_label_create(
            pressureItem
        );


    // ========================================================
    // SUNRISE ITEM
    // ========================================================

    sunriseItem =
        lv_obj_create(bottomInfo);


    lv_obj_remove_style_all(
        sunriseItem
    );


    lv_obj_set_height(
        sunriseItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_width(
        sunriseItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_flex_flow(
        sunriseItem,
        LV_FLEX_FLOW_ROW
    );


    lv_obj_set_flex_align(
        sunriseItem,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );


    sunriseIcon =
        lv_label_create(
            sunriseItem
        );


    sunrise =
        lv_label_create(
            sunriseItem
        );


    // ========================================================
    // SUNSET ITEM
    // ========================================================

    sunsetItem =
        lv_obj_create(bottomInfo);


    lv_obj_remove_style_all(
        sunsetItem
    );


    lv_obj_set_height(
        sunsetItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_width(
        sunsetItem,
        LV_SIZE_CONTENT
    );


    lv_obj_set_flex_flow(
        sunsetItem,
        LV_FLEX_FLOW_ROW
    );


    lv_obj_set_flex_align(
        sunsetItem,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER
    );


    sunsetIcon =
        lv_label_create(
            sunsetItem
        );


    sunset =
        lv_label_create(
            sunsetItem
        );


    // ========================================================
    // INFO ICONS
    // ========================================================

    lv_label_set_text(
        humidityIcon,
        LV_SYMBOL_TINT
    );


    lv_label_set_text(
        windIcon,
        LV_SYMBOL_REFRESH
    );


    lv_label_set_text(
        pressureIcon,
        LV_SYMBOL_SETTINGS
    );


    lv_label_set_text(
        sunriseIcon,
        LV_SYMBOL_UP
    );


    lv_label_set_text(
        sunsetIcon,
        LV_SYMBOL_DOWN
    );


    // ========================================================
    // ICON FONTS
    // ========================================================

    lv_obj_set_style_text_font(
        humidityIcon,
        &montserrat_28_es_18,
        0
    );


    lv_obj_set_style_text_font(
        windIcon,
        &montserrat_28_es_18,
        0
    );


    lv_obj_set_style_text_font(
        pressureIcon,
        &montserrat_28_es_18,
        0
    );


    lv_obj_set_style_text_font(
        sunriseIcon,
        &montserrat_28_es_18,
        0
    );


    lv_obj_set_style_text_font(
        sunsetIcon,
        &montserrat_28_es_18,
        0
    );


    // ========================================================
    // INFO TEXT FONTS
    // ========================================================

    lv_obj_set_style_text_font(
        humidity,
        &montserrat_28_es_16,
        0
    );


    lv_obj_set_style_text_font(
        wind,
        &montserrat_28_es_16,
        0
    );


    lv_obj_set_style_text_font(
        pressure,
        &montserrat_28_es_16,
        0
    );


    lv_obj_set_style_text_font(
        sunrise,
        &montserrat_28_es_16,
        0
    );


    lv_obj_set_style_text_font(
        sunset,
        &montserrat_28_es_16,
        0
    );


    // ========================================================
    // INITIAL VALUES
    // ========================================================

    lv_label_set_text(
        humidity,
        ""
    );


    lv_label_set_text(
        wind,
        ""
    );


    lv_label_set_text(
        pressure,
        ""
    );


    lv_label_set_text(
        sunrise,
        ""
    );


    lv_label_set_text(
        sunset,
        ""
    );


    // ========================================================
    // ICONO ESTATICO (sin animacion)
    // ========================================================

    /*
     * Se probo animando a ~12 fps, pero decodificar PNG cada
     * 83ms compite por CPU con el resto de la UI y se notaba
     * en los cambios de pantalla. Con el hardware disponible,
     * fluidez del "centro de control" > animacion bonita del
     * icono. configureScene() ya deja el frame 1 puesto; aqui
     * simplemente no arrancamos ningun timer que lo cambie.
     */
}


// ============================================================
// DESTRUCTOR
// ============================================================

WeatherCard::~WeatherCard()
{
    if (animationTimer)
    {
        lv_timer_delete(
            animationTimer
        );

        animationTimer = nullptr;
    }
}


// ============================================================
// LAYOUT
// ============================================================

void WeatherCard::layout()
{
    // ========================================================
    // WEATHER HUD
    // ========================================================

    // --------------------------------------------------------
    // LOCATION
    // --------------------------------------------------------

    lv_obj_set_pos(
        location,
        35,
        28
    );


    // --------------------------------------------------------
    // TEMPERATURE
    // --------------------------------------------------------

    lv_obj_set_pos(
        temperature,
        35,
        70
    );


    // --------------------------------------------------------
    // DESCRIPTION
    // --------------------------------------------------------

    lv_obj_set_pos(
        description,
        35,
        125
    );


    // ========================================================
    // BOTTOM INFORMATION
    // ========================================================

    /*
     * bottomInfo ocupa toda la anchura de la card.
     *
     * Los cinco elementos internos se distribuyen
     * automáticamente mediante SPACE_BETWEEN.
     */

    lv_obj_align(
        bottomInfo,
        LV_ALIGN_BOTTOM_MID,
        0,
        -15
    );


    // ========================================================
    // ITEM SPACING
    // ========================================================

    /*
     * Cada Item contiene:
     *
     *     [icono] [dato]
     *
     * Dejamos unos píxeles entre ambos.
     */

    lv_obj_set_style_pad_column(
        humidityItem,
        6,
        0
    );


    lv_obj_set_style_pad_column(
        windItem,
        6,
        0
    );


    lv_obj_set_style_pad_column(
        pressureItem,
        6,
        0
    );


    lv_obj_set_style_pad_column(
        sunriseItem,
        6,
        0
    );


    lv_obj_set_style_pad_column(
        sunsetItem,
        6,
        0
    );


    // ========================================================
    // ICON
    // ========================================================

    lv_obj_align(
        icon,
        LV_ALIGN_TOP_RIGHT,
        -(ICON_MARGIN_RIGHT + ICON_OVERFLOW),
        ICON_TOP + ICON_OVERFLOW
    );


    // ========================================================
    // TEXT COLORS
    // ========================================================

    lv_obj_set_style_text_color(
        location,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        temperature,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        description,
        Theme::Text(),
        0
    );


    // --------------------------------------------------------
    // INFORMATION TEXT
    // --------------------------------------------------------

    lv_obj_set_style_text_color(
        humidity,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        wind,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        pressure,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        sunrise,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        sunset,
        Theme::Text(),
        0
    );


    // --------------------------------------------------------
    // INFORMATION ICONS
    // --------------------------------------------------------

    lv_obj_set_style_text_color(
        humidityIcon,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        windIcon,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        pressureIcon,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        sunriseIcon,
        Theme::Text(),
        0
    );


    lv_obj_set_style_text_color(
        sunsetIcon,
        Theme::Text(),
        0
    );
}


// ============================================================
// SETTERS
// ============================================================

void WeatherCard::setLocation(
    const char* txt
)
{
    lv_label_set_text(
        location,
        txt ? txt : ""
    );
}


// ============================================================
// TEMPERATURE
// ============================================================

void WeatherCard::setTemperature(
    const char* txt
)
{
    lv_label_set_text(
        temperature,
        txt ? txt : ""
    );
}


// ============================================================
// DESCRIPTION
// ============================================================

void WeatherCard::setDescription(
    const char* txt
)
{
    lv_label_set_text(
        description,
        txt ? txt : ""
    );
}


// ============================================================
// WEATHER ICON
// ============================================================

void WeatherCard::setIcon(
    const char* txt
)
{
    /*
     * El icono de OpenWeather no se muestra directamente.
     *
     * Lo utilizamos para determinar la escena
     * meteorológica que debe dibujarse.
     */

    updateSceneFromIcon(
        txt
    );

    lv_obj_invalidate(
        card
    );
}


// ============================================================
// HUMIDITY
// ============================================================

void WeatherCard::setHumidity(
    int value
)
{
    char buffer[32];


    snprintf(
        buffer,
        sizeof(buffer),
        "%d%%",
        value
    );


    lv_label_set_text(
        humidity,
        buffer
    );
}


// ============================================================
// WIND
// ============================================================

void WeatherCard::setWind(
    float value
)
{
    char buffer[32];


    snprintf(
        buffer,
        sizeof(buffer),
        "%.0f km/h",
        value
    );


    lv_label_set_text(
        wind,
        buffer
    );
}


// ============================================================
// PRESSURE
// ============================================================

void WeatherCard::setPressure(
    int value
)
{
    char buffer[32];


    snprintf(
        buffer,
        sizeof(buffer),
        "%d hPa",
        value
    );


    lv_label_set_text(
        pressure,
        buffer
    );
}


// ============================================================
// SUNRISE
// ============================================================

void WeatherCard::setSunrise(
    const char* value
)
{
    /*
     * ApiService ya ha convertido el timestamp
     * de OpenWeather a HH:MM.
     *
     * Ejemplo:
     *
     *     1786081234
     *
     *          ↓
     *
     *     "07:32"
     *
     * WeatherCard solamente lo muestra.
     */

    lv_label_set_text(
        sunrise,
        value ? value : ""
    );
}


// ============================================================
// SUNSET
// ============================================================

void WeatherCard::setSunset(
    const char* value
)
{
    /*
     * Igual que sunrise:
     *
     * ApiService -> "21:18"
     * WeatherCard -> mostrarlo.
     */

    lv_label_set_text(
        sunset,
        value ? value : ""
    );
}


// ============================================================
// ANIMATION TIMER CALLBACK
// ============================================================

void WeatherCard::frameTimerCallback(
    lv_timer_t* timer
)
{
    WeatherCard* self =
        static_cast<WeatherCard*>(
            lv_timer_get_user_data(
                timer
            )
        );

    if (!self || self->frameCount <= 1)
        return;

    self->frameIndex =
        (self->frameIndex + 1) % self->frameCount;

    self->updateFrameSrc();
}


// ============================================================
// FRAME SOURCE
// ============================================================

void WeatherCard::updateFrameSrc()
{
    char path[96];

    snprintf(
        path,
        sizeof(path),
        "L:/weather/%s/frame_%03d.png",
        frameFolder,
        frameIndex + 1
    );

    // DEBUG temporal: confirmar que el fichero existe de verdad
    // en LittleFS con la ruta exacta que le pasamos a LVGL.
    char fsPath[96];

    snprintf(
        fsPath,
        sizeof(fsPath),
        "/weather/%s/frame_%03d.png",
        frameFolder,
        frameIndex + 1
    );

    Serial.printf(
        "[WeatherCard] escena=%s path=%s existe=%s\n",
        frameFolder,
        path,
        LittleFS.exists(fsPath) ? "SI" : "NO"
    );

    lv_image_set_src(
        icon,
        path
    );
}


// ============================================================
// SCENE CONFIGURATION
// ============================================================

void WeatherCard::configureScene(
    WeatherScene newScene
)
{
    struct SceneInfo
    {
        const char* folder;
        int frames;
        bool hasSky;
        uint32_t sky;
    };

    SceneInfo info;

        switch (newScene)
    {
        case WeatherScene::ClearDay:
            info = { "clear-day", 1, true, 0x2980B9 };
            break;

        case WeatherScene::ClearNight:
            info = { "clear-night", 1, true, 0x101B3D };
            break;

        case WeatherScene::PartlyCloudyNight:
            info = { "partly-cloudy-night", 1, true, 0x131F42 };
            break;

        case WeatherScene::CloudyNight:
            info = { "cloudy-night", 1, true, 0x1A2340 };
            break;

        case WeatherScene::RainNight:
            info = { "rain-night", 1, true, 0x141C32 };
            break;

        case WeatherScene::StormNight:
            info = { "thunderstorms-night", 1, true, 0x0E1526 };
            break;

        case WeatherScene::SnowNight:
            info = { "snow-night", 1, true, 0x1C2842 };
            break;
        case WeatherScene::PartlyCloudy:
            info = { "overcast", 1, true, 0x90A4AE };
            break;
        case WeatherScene::Cloudy:
            info = { "cloudy", 1, true, 0x607D8B };
            break;

        case WeatherScene::Rain:
            info = { "rain", 1, true, 0x455A64 };
            break;

        case WeatherScene::Storm:
            info = { "thunderstorms-day-rain", 1, true, 0x263238 };
            break;

        case WeatherScene::Snow:
            info = { "snow", 1, true, 0x78909C };
            break;

        case WeatherScene::Unknown:
        default:
            info = { "not-available", 1, false, 0 };
            break;
    }

    strncpy(
        frameFolder,
        info.folder,
        sizeof(frameFolder) - 1
    );

    frameFolder[sizeof(frameFolder) - 1] = '\0';

    frameCount = info.frames;
    frameIndex = 0;

    lv_obj_set_style_bg_color(
        card,
        info.hasSky ? lv_color_hex(info.sky) : Theme::Weather(),
        0
    );

    updateFrameSrc();
}


// ============================================================
// UPDATE SCENE FROM ICON
// ============================================================

void WeatherCard::updateSceneFromIcon(
    const char* iconCode
)
{
    /*
     * Calculamos la escena en una variable local y solo
     * reconfiguramos (fondo + carpeta de frames + reinicio
     * de animación) si realmente ha cambiado respecto a la
     * anterior. Sin esto, cada refresco de datos de la API
     * (aunque el clima siga igual) reiniciaría la animación
     * desde el frame 0, dando una sensación de "salto".
     */

    WeatherScene newScene =
        WeatherScene::Unknown;


    if (
        !iconCode ||
        strlen(iconCode) < 2
    )
    {
        newScene = WeatherScene::Unknown;

        if (newScene != scene)
        {
            scene = newScene;
            configureScene(scene);
        }

        return;
    }


    // ========================================================
    // NOCHE: cualquier icono terminado en "n". Cada tiempo tiene
    // su propia escena nocturna (luna + nubes, lluvia, tormenta,
    // nieve); el cielo despejado y lo desconocido usan la luna sola.
    // ========================================================
    size_t len = strlen(iconCode);
    if (iconCode[len - 1] == 'n')
    {
        if (strncmp(iconCode, "02", 2) == 0)
            newScene = WeatherScene::PartlyCloudyNight;
        else if (strncmp(iconCode, "03", 2) == 0 ||
                 strncmp(iconCode, "04", 2) == 0 ||
                 strncmp(iconCode, "50", 2) == 0)
            newScene = WeatherScene::CloudyNight;
        else if (strncmp(iconCode, "09", 2) == 0 ||
                 strncmp(iconCode, "10", 2) == 0)
            newScene = WeatherScene::RainNight;
        else if (strncmp(iconCode, "11", 2) == 0)
            newScene = WeatherScene::StormNight;
        else if (strncmp(iconCode, "13", 2) == 0)
            newScene = WeatherScene::SnowNight;
        else
            newScene = WeatherScene::ClearNight;
    }


    // ========================================================
    // CLEAR DAY
    // ========================================================

    else if (
        strcmp(iconCode, "01d") == 0
    )
    {
        newScene =
            WeatherScene::ClearDay;
    }


    // ========================================================
    // PARTLY CLOUDY (pocas nubes, con el sol todavia visible)
    // ========================================================

    else if (
        strcmp(iconCode, "02d") == 0
    )
    {
        newScene =
            WeatherScene::PartlyCloudy;
    }


    // ========================================================
    // CLOUDY (nubes dispersas / cielo cubierto, sin sol visible)
    // ========================================================

    else if (
        strcmp(iconCode, "03d") == 0 ||
        strcmp(iconCode, "04d") == 0
    )
    {
        newScene =
            WeatherScene::Cloudy;
    }


    // ========================================================
    // RAIN
    // ========================================================

    else if (
        strcmp(iconCode, "09d") == 0 ||
        strcmp(iconCode, "10d") == 0
    )
    {
        newScene =
            WeatherScene::Rain;
    }


    // ========================================================
    // STORM
    // ========================================================

    else if (
        strcmp(iconCode, "11d") == 0
    )
    {
        newScene =
            WeatherScene::Storm;
    }


    // ========================================================
    // SNOW
    // ========================================================

    else if (
        strcmp(iconCode, "13d") == 0
    )
    {
        newScene =
            WeatherScene::Snow;
    }


    // ========================================================
    // MIST / FOG / HAZE / SMOKE / DUST / SAND / ASH / SQUALL / TORNADO
    // ========================================================

    else if (
        strcmp(iconCode, "50d") == 0
    )
    {
        newScene =
            WeatherScene::Cloudy;
    }


    // ========================================================
    // UNKNOWN
    // ========================================================

    else
    {
        newScene =
            WeatherScene::Unknown;
    }

    if (newScene != scene)
    {
        scene = newScene;
        configureScene(scene);
    }
}



// ============================================================
// PAUSE / RESUME
// ============================================================

/*
 * Cuando la card no es visible (pantalla oculta) no tiene
 * sentido seguir decodificando PNG 12 veces por segundo.
 */

void WeatherCard::pause()
{
    if (animationTimer)
    {
        lv_timer_pause(animationTimer);
    }
}


void WeatherCard::resume()
{
    if (animationTimer)
    {
        lv_timer_resume(animationTimer);
    }
}
