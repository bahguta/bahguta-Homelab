#include <Arduino.h>
#include "app/Application.h"

Application app;

void setup()
{
    app.setup();
}

void loop()
{
    /*
     * Vacio a proposito: Application::setup() termina con
     * powerManager.sleep(), que entra en deep sleep y reinicia
     * el chip -- loop() nunca llega a ejecutarse de verdad. Se
     * deja vacio porque el framework de Arduino exige que exista.
     */
}
