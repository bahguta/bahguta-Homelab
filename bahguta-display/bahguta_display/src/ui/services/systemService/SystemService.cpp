#include "SystemService.h"

#include "../ServiceManager.h"
#include <Arduino.h>

#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


// ============================================================
// INIT
// ============================================================

void SystemService::init()
{
    // ========================================================
    // BATTERY ADC
    // ========================================================

    analogReadResolution(12);

    analogSetPinAttenuation(
        BATTERY_PIN,
        ADC_11db
    );


    // ========================================================
    // CPU MONITORING
    // ========================================================

    //lastCpuRuntime = 0;
    //lastCpuTimestamp = esp_timer_get_time();

    //systemInfo.tabletCpu = 0.0;
}


// ============================================================
// UPDATE
// ============================================================

void SystemService::update()
{
    // ========================================================
    // CPU
    // ========================================================

    // systemInfo.tabletCpu =
    //     calculateCpuUsage();


    // ========================================================
    // RAM
    // ========================================================

    const uint32_t totalHeap =
        heap_caps_get_total_size(
            MALLOC_CAP_INTERNAL
        );

    const uint32_t freeHeap =
        heap_caps_get_free_size(
            MALLOC_CAP_INTERNAL
        );


    if (totalHeap > 0)
    {
        systemInfo.ram =
            (
                (double)(
                    totalHeap -
                    freeHeap
                )
                /
                totalHeap
            )
            *
            100.0;
    }


    // ========================================================
    // UPTIME
    // ========================================================

    systemInfo.uptime =
        millis() / 1000;


    // ========================================================
    // BATTERY
    // ========================================================

    double voltage =
        analogReadMilliVolts(
            BATTERY_PIN
        )
        /
        1000.0;


    /*
     * Filtro mucho mas fuerte que antes (0.9/0.1 -> 0.97/0.03).
     * Con solo 0.63V de rango total, cada 1% son ~6mV -- el
     * ruido normal del ADC se traga esa resolucion facilmente
     * si no se suaviza mucho mas de lo que parece necesario.
     */

    batteryFiltered =
        (batteryFiltered * 0.97)
        +
        (voltage * 0.03);


    double percentage =
        (
            (
                batteryFiltered -
                BATTERY_MIN_VOLTAGE
            )
            /
            (
                BATTERY_MAX_VOLTAGE -
                BATTERY_MIN_VOLTAGE
            )
        )
        *
        100.0;


    percentage =
        constrain(
            percentage,
            0.0,
            100.0
        );


    int rawBattery =
        (int)round(
            percentage
        );

    /*
     * Histeresis: solo actualizamos el valor MOSTRADO si se
     * mueve al menos 2 puntos respecto al ultimo que ensenamos.
     * Sin esto, un valor real oscilando justo en la mitad
     * (ej. 79.5%) salta entre 79 y 80 con cualquier ruidito,
     * aunque el voltaje filtrado apenas se mueva.
     */

    if (
        lastReportedBattery < 0 ||
        abs(rawBattery - lastReportedBattery) >= 2
    )
    {
        lastReportedBattery = rawBattery;
    }

    systemInfo.battery = lastReportedBattery;


    // ========================================================
    // APAGADO PREVENTIVO
    // ========================================================

    /*
     * En vez de esperar a que el propio circuito de proteccion
     * de la bateria corte la alimentacion en seco, apagamos
     * nosotros de forma ordenada un poco antes. Usamos el valor
     * YA filtrado por la histeresis de arriba, no la lectura
     * cruda, para no dispararlo por un pico de ruido puntual.
     */

    if (batteryVoltage < BATTERY_NOT_PRESENT_VOLTAGE)
    {
        /*
         * No hay bateria conectada de verdad (voltaje demasiado
         * bajo hasta para estar "vacia") -- va por USB. No
         * disparamos el apagado por esto.
         */
        lowBatteryCount = 0;
    }
    else if (systemInfo.battery <= LOW_BATTERY_SHUTDOWN_PERCENT)
    {
        lowBatteryCount++;

        /*
         * Exigimos varias lecturas bajas seguidas (no solo la
         * primera nada mas arrancar) antes de apagar de verdad.
         */
        if (lowBatteryCount >= 5)
        {
            Serial.println("Bateria critica -- apagando de forma ordenada.");

            ServiceManager::power().shutdown();
        }
    }
    else
    {
        lowBatteryCount = 0;
    }


    // ========================================================
    // CHARGING
    // ========================================================

    /*
     * Por ahora no tenemos una señal fiable
     * de estado de carga.
     */

    systemInfo.charging =
        false;
}


// ============================================================
// CPU USAGE
// ============================================================

// double SystemService::calculateCpuUsage()
// {
//     /*
//      * FreeRTOS mantiene estadísticas acumuladas del tiempo
//      * de ejecución de cada tarea.
//      *
//      * Buscamos las tareas Idle de los cores y calculamos:
//      *
//      *     CPU = 100 - Idle%
//      *
//      * Esto nos proporciona una medida real de utilización
//      * del procesador en lugar de una estimación artificial.
//      */


// #if CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS

//     const UBaseType_t taskCount =
//         uxTaskGetNumberOfTasks();


//     if (taskCount == 0)
//     {
//         return 0.0;
//     }


//     /*
//      * Reservamos memoria para obtener el estado
//      * de las tareas.
//      */

//     TaskStatus_t* taskStatus =
//         static_cast<TaskStatus_t*>(
//             pvPortMalloc(
//                 taskCount *
//                 sizeof(TaskStatus_t)
//             )
//         );


//     if (!taskStatus)
//     {
//         return 0.0;
//     }


//     configRUN_TIME_COUNTER_TYPE totalRuntime =
//         0;


//     const UBaseType_t count =
//         uxTaskGetSystemState(
//             taskStatus,
//             taskCount,
//             &totalRuntime
//         );


//     if (
//         count == 0 ||
//         totalRuntime == 0
//     )
//     {
//         vPortFree(taskStatus);

//         return 0.0;
//     }


//     /*
//      * Sumamos el tiempo consumido por las tareas Idle.
//      *
//      * En ESP32-P4 normalmente tendremos dos cores,
//      * por lo que habrá un Idle task por core.
//      */

//     configRUN_TIME_COUNTER_TYPE idleRuntime =
//         0;


//     for (UBaseType_t i = 0; i < count; i++)
//     {
//         const char* name =
//             taskStatus[i].pcTaskName;


//         if (!name)
//             continue;


//         /*
//          * Las tareas Idle de ESP-IDF tienen nombres
//          * del tipo:
//          *
//          *     IDLE0
//          *     IDLE1
//          */

//         if (
//             strncmp(
//                 name,
//                 "IDLE",
//                 4
//             ) == 0
//         )
//         {
//             idleRuntime +=
//                 taskStatus[i].ulRunTimeCounter;
//         }
//     }


//     vPortFree(taskStatus);


//     /*
//      * Evitamos cualquier división inválida.
//      */

//     if (idleRuntime > totalRuntime)
//     {
//         idleRuntime =
//             totalRuntime;
//     }


//     const double idlePercentage =
//         (
//             (double)idleRuntime
//             /
//             (double)totalRuntime
//         )
//         *
//         100.0;


//     double cpu =
//         100.0 -
//         idlePercentage;


//     cpu =
//         constrain(
//             cpu,
//             0.0,
//             100.0
//         );


//     return cpu;

// #else

//     /*
//      * Si las runtime stats no están habilitadas,
//      * no podemos obtener una medición real.
//      *
//      * No inventamos un valor.
//      */

//     return 0.0;

// #endif
// }


// ============================================================
// BATTERY PERCENTAGE
// ============================================================

int SystemService::calculateBatteryPercentage(
    float voltage)
{
    float percentage =
        (
            (
                voltage -
                BATTERY_MIN_VOLTAGE
            )
            /
            (
                BATTERY_MAX_VOLTAGE -
                BATTERY_MIN_VOLTAGE
            )
        )
        *
        100.0f;


    // ========================================================
    // LIMIT
    // ========================================================

    if (percentage < 0.0f)
        percentage = 0.0f;


    if (percentage > 100.0f)
        percentage = 100.0f;


    return (int)(
        percentage +
        0.5f
    );
}


// ============================================================
// SYSTEM INFO
// ============================================================

SystemInfo& SystemService::system()
{
    return systemInfo;
}