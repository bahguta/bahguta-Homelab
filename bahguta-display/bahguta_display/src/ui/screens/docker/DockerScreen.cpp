#include "DockerScreen.h"
#include "../../../fonts/Fonts.h"

#include "../../themes/Theme.h"
#include "../../layout/ContentLayout.h"
#include "../../services/ServiceManager.h"
#include "../../models/DockerInfo.h"
#include "../../models/DockerContainerInfo.h"

#include <Arduino.h>
#include <string.h>


static constexpr int LIST_TOP = ContentLayout::CONTENT_TOP;

static constexpr int LIST_BOTTOM_MARGIN = 15;
static constexpr int SCREEN_HEIGHT_DEFAULT = 970;

static constexpr int ROW_HEIGHT = 64;
static constexpr int ROW_GAP = 8;

static constexpr int DOT_SIZE = 12;

// Tras tocar un switch no se corrige su posicion hasta pasado este tiempo: la accion
// tarda en llegar al estado real y, si no, el switch saltaria atras y adelante
static constexpr uint32_t TOGGLE_SETTLE_MS = 8000;
static uint32_t toggleSettleUntil = 0;

static constexpr int NAME_X = 30;
static constexpr int STATE_RIGHT = 95;


void DockerScreen::create(lv_obj_t* parent)
{
    screen = lv_obj_create(parent);

    lv_obj_remove_style_all(screen);

    lv_obj_set_size(
        screen,
        LV_PCT(100),
        LV_PCT(100)
    );

    lv_obj_set_pos(
        screen,
        0,
        0
    );

    // La PANTALLA no hace scroll: solo la lista. Antes la pantalla si podia
    // deslizarse (la lista medida 1080 px desbordaba el area de 970), asi que
    // se arrastraba todo el contenido mas alla de la lista y cada fotograma
    // redibujaba 800 x 970 px.
    lv_obj_clear_flag(
        screen,
        LV_OBJ_FLAG_SCROLLABLE
    );

    lv_obj_set_style_bg_opa(
        screen,
        LV_OPA_COVER,
        0
    );

    lv_obj_set_style_bg_color(
        screen,
        Theme::Background(),
        0
    );

    // Resolver el tamano de la pantalla antes de medirla (como HomeScreen)
    lv_obj_update_layout(screen);

    createContent();
}


int DockerScreen::listHeight() const
{
    int h = lv_obj_get_height(screen);

    if (h <= 400)
        h = SCREEN_HEIGHT_DEFAULT;

    return h - LIST_TOP - LIST_BOTTOM_MARGIN;
}


void DockerScreen::createContent()
{
    ContentLayout::createTitle(screen, "Docker");


    // =========================
    // LIST
    // =========================

    list = lv_obj_create(screen);

    lv_obj_remove_style_all(list);

    lv_obj_set_size(
        list,
        ContentLayout::screenContentWidth(screen),
        listHeight()
    );

    lv_obj_set_pos(
        list,
        ContentLayout::LEFT,
        LIST_TOP
    );

    lv_obj_set_style_bg_opa(
        list,
        LV_OPA_TRANSP,
        0
    );

    /*
     * La lista sí puede hacer scroll.
     *
     * La pantalla completa no.
     */
    lv_obj_add_flag(
        list,
        LV_OBJ_FLAG_SCROLLABLE
    );

    // Scroll solo vertical, sin rebote ni inercia: cada fotograma extra es
    // un redibujado completo de la lista (renderizado por software) y el
    // rebote dejaba deslizar mas alla del ultimo contenedor.
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLL_MOMENTUM);

    lv_obj_set_scrollbar_mode(
        list,
        LV_SCROLLBAR_MODE_OFF
    );

    createContainerList();
}


void DockerScreen::createContainerList()
{
    for (int i = 0; i < 32; i++)
    {
        // =========================
        // ROW
        // =========================

        containerRows[i] =
            lv_obj_create(list);

        lv_obj_remove_style_all(
            containerRows[i]
        );

        // Las filas no hacen scroll por si mismas (solo la lista)
        lv_obj_clear_flag(
            containerRows[i],
            LV_OBJ_FLAG_SCROLLABLE
        );

        lv_obj_set_size(
            containerRows[i],
            LV_PCT(100),
            ROW_HEIGHT
        );

        lv_obj_set_pos(
            containerRows[i],
            0,
            i * (ROW_HEIGHT + ROW_GAP)
        );

        lv_obj_set_style_bg_color(
            containerRows[i],
            Theme::Panel(),
            0
        );

        lv_obj_set_style_bg_opa(
            containerRows[i],
            LV_OPA_COVER,
            0
        );

        lv_obj_set_style_radius(
            containerRows[i],
            12,
            0
        );

        lv_obj_add_flag(
            containerRows[i],
            LV_OBJ_FLAG_HIDDEN
        );


        // =========================
        // STATUS DOT
        // =========================

        statusDots[i] =
            lv_obj_create(containerRows[i]);

        lv_obj_remove_style_all(
            statusDots[i]
        );

        lv_obj_set_size(
            statusDots[i],
            DOT_SIZE,
            DOT_SIZE
        );

        lv_obj_set_style_radius(
            statusDots[i],
            LV_RADIUS_CIRCLE,
            0
        );

        lv_obj_set_style_bg_color(
            statusDots[i],
            Theme::Text(),
            0
        );

        lv_obj_set_style_bg_opa(
            statusDots[i],
            LV_OPA_COVER,
            0
        );

        lv_obj_align(
            statusDots[i],
            LV_ALIGN_LEFT_MID,
            NAME_X,
            0
        );


        // =========================
        // NAME
        // =========================

        nameLabels[i] =
            lv_label_create(containerRows[i]);

        lv_obj_set_style_text_font(
            nameLabels[i],
            &montserrat_28_es_20,
            0
        );

        lv_obj_set_style_text_color(
            nameLabels[i],
            Theme::Text(),
            0
        );

        lv_obj_set_pos(
            nameLabels[i],
            NAME_X + 25,
            18
        );


        // =========================
        // STATE
        // =========================

        stateLabels[i] =
            lv_label_create(containerRows[i]);

        lv_obj_set_style_text_font(
            stateLabels[i],
            &montserrat_28_es_16,
            0
        );

        lv_obj_set_style_text_color(
            stateLabels[i],
            Theme::Text(),
            0
        );

        lv_obj_align(
            stateLabels[i],
            LV_ALIGN_RIGHT_MID,
            -STATE_RIGHT,
            0
        );


        // =========================
        // SWITCH
        // =========================

        switches[i] = lv_switch_create(
            containerRows[i]
        );

        lv_obj_set_size(
            switches[i],
            58,
            30
        );

        lv_obj_align(
            switches[i],
            LV_ALIGN_RIGHT_MID,
            -20,
            0
        );

        lv_obj_add_event_cb(
            switches[i],
            toggleEvent,
            LV_EVENT_VALUE_CHANGED,
            containerIds[i]
        );
    

    }
}


void DockerScreen::update()
{
    updateContainerList();
}


void DockerScreen::updateContainerList()
{
    const DockerInfo& docker =
        ServiceManager::docker().docker();

    int count = docker.containerCount;

    if (count < 0)
        count = 0;

    if (count > 32)
        count = 32;


    // =========================
    // UPDATE VISIBLE ROWS
    // =========================

    for (int i = 0; i < count; i++)
    {
        const DockerContainerInfo& container =
            docker.containers[i];

        // -------------------------
        // ROW (visible)
        // -------------------------
        if (lv_obj_has_flag(containerRows[i], LV_OBJ_FLAG_HIDDEN))
        {
            lv_obj_clear_flag(
                containerRows[i],
                LV_OBJ_FLAG_HIDDEN
            );
        }

        // -------------------------
        // NAME (solo si cambio)
        // -------------------------
        if (strcmp(lv_label_get_text(nameLabels[i]), container.name.c_str()) != 0)
        {
            lv_label_set_text(
                nameLabels[i],
                container.name.c_str()
            );
        }

        strncpy(
            containerIds[i],
            container.id.c_str(),
            sizeof(containerIds[i]) - 1
        );
        containerIds[i][sizeof(containerIds[i]) - 1] = '\0';

        // -------------------------
        // STATE
        // -------------------------
        const bool running =
            container.state == "running";

        // El switch se corrige si NO coincide con el dato real (p. ej. una
        // accion que fallo), no en cada refresco
        const bool switchOn =
            lv_obj_has_state(switches[i], LV_STATE_CHECKED);

        if (switchOn != running && (int32_t)(millis() - toggleSettleUntil) >= 0)
        {
            if (running)
                lv_obj_add_state(switches[i], LV_STATE_CHECKED);
            else
                lv_obj_clear_state(switches[i], LV_STATE_CHECKED);
        }

        // Texto y punto de color: solo si cambia el estado
        if (shownRunning[i] != (running ? 1 : 0))
        {
            shownRunning[i] = running ? 1 : 0;

            lv_label_set_text(
                stateLabels[i],
                running ? "RUNNING" : "STOPPED"
            );

            lv_obj_set_style_bg_color(
                statusDots[i],
                running ? lv_color_make(60, 200, 90)
                        : lv_color_make(110, 110, 110),
                0
            );
        }
    }

    // =========================
    // HIDE UNUSED ROWS
    // =========================

    for (int i = count; i < 32; i++)
    {
        if (!lv_obj_has_flag(containerRows[i], LV_OBJ_FLAG_HIDDEN))
        {
            lv_obj_add_flag(
                containerRows[i],
                LV_OBJ_FLAG_HIDDEN
            );
        }

        shownRunning[i] = -1;
    }

    rowCount = count;
}


void DockerScreen::show()
{
    lv_obj_clear_flag(
        screen,
        LV_OBJ_FLAG_HIDDEN
    );

    update();
}


void DockerScreen::hide()
{
    lv_obj_add_flag(
        screen,
        LV_OBJ_FLAG_HIDDEN
    );
}


void DockerScreen::destroy()
{
    if (screen)
    {
        lv_obj_delete(screen);

        screen = nullptr;
    }

    list = nullptr;

    for (int i = 0; i < 32; i++)
    {
        containerRows[i] = nullptr;
        statusDots[i] = nullptr;
        nameLabels[i] = nullptr;
        stateLabels[i] = nullptr;
        switches[i] = nullptr;
    }

    rowCount = 0;
}

// ============================================================
// TOGGLE (start/stop)
// ============================================================

/*
 * IMPORTANTE (backend): endpoints propuestos, pendientes de
 * que los declares:
 *
 *   POST /api/v1/system/docker/container/{id}/start
 *   POST /api/v1/system/docker/container/{id}/stop
 *
 * Respuesta esperada: el DockerContainerDto actualizado,
 * envuelto igual que el resto ({"success":true,"data":{...}}).
 * No hace falta que la respuesta traiga nada mas -- el
 * siguiente refresco periodico de la lista (cada 30s) ya
 * corrige el estado si el arranque/parada tarda un poco.
 */

void DockerScreen::toggleEvent(lv_event_t* e)
{
    lv_obj_t* sw =
        static_cast<lv_obj_t*>(lv_event_get_target(e));

    const char* containerId =
        static_cast<const char*>(lv_event_get_user_data(e));

    if (!containerId || strlen(containerId) == 0)
        return;

    const bool wantsRunning =
        lv_obj_has_state(sw, LV_STATE_CHECKED);

    Serial.printf(
        "DOCKER TOGGLE -------- id=%s -> %s\n",
        containerId,
        wantsRunning ? "start" : "stop"
    );

    /*
     * La accion la envia la task de runtime (no bloquea la UI). El switch
     * se queda donde lo puso el usuario; el estado real llega con el
     * siguiente refresco y, si la accion fallo, el switch vuelve a su sitio.
     */
    toggleSettleUntil = millis() + TOGGLE_SETTLE_MS;

    ServiceManager::api().requestContainerAction(containerId, wantsRunning);
}
