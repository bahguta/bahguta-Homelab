# bahguta display (tablet ESP32-P4)

🇬🇧 [English](README.md) · 🇪🇸 Español

Firmware de la tablet táctil: un sketch de Arduino con interfaz LVGL que consulta el dashboard del backend y dibuja las pantallas (inicio, casa, tiempo, Docker, dispositivos, red, sistema y ajustes).

## Hardware

- Placa ESP32-P4 con **pantalla de 800x1280** (driver JD9365, en `src/lcd/`) y **controlador táctil GSL3680** (`src/touch/`). Los pines están en `pins_config.h`.
- **16 MB de flash**: `partitions.csv` define dos ranuras OTA para la aplicación y una partición LittleFS de 8 MB (con la etiqueta `spiffs`).

## Requisitos

- Arduino IDE o `arduino-cli` con el **core de Arduino para ESP32 v3.1**. Otras versiones no están soportadas.
- La librería **LVGL 9.2** para Arduino (el `lv_conf.h` de esta carpeta está escrito para la v9.2.2). Copia el `lv_conf.h` de esta carpeta a la carpeta `libraries/` de Arduino, junto a `lvgl/`, como piden las instrucciones de LVGL para Arduino.
- Una herramienta para subir una carpeta a LittleFS desde el Arduino IDE.

## Compilar y grabar

1. Copia `src/ui/Secrets.example.h` a `src/ui/Secrets.h` y pon en `BAHGUTA_API_KEY` el mismo valor que en el `.env` del backend. `Secrets.h` está en `.gitignore`: no lo subas nunca. Sin la clave, arrancar y parar contenedores falla con `401`.
2. Abre `bahguta_display.ino`, elige tu placa ESP32-P4 y la **tabla de particiones personalizada** (`partitions.csv`, en esta carpeta), compila y sube.
3. Sube la carpeta `data/` (`icons`, `weather`, `weather-mini`, `weather-slot`) a la partición **LittleFS**.
4. En el primer arranque entra en **Ajustes** y escribe el WiFi y la URL del backend (por ejemplo `http://bahguta.local`). Se guardan en la tablet, no en el código.

## Estructura

```
bahguta_display.ino   punto de entrada: pantalla, táctil, LittleFS y arranque de la UI
src/lcd, src/touch    drivers de pantalla y táctil
src/ui/layout         layout principal: cabecera, pie, menú inferior y vistas
src/ui/screens        un fichero por pantalla
src/ui/services       acceso al backend, ajustes, hora y el resto de servicios
src/ui/widgets        componentes reutilizables (tarjetas, interruptores...)
src/ui/themes         colores y estilos
data/                 iconos y animaciones del tiempo (se suben a LittleFS)
```

Para contribuir: mantén el código fluido (sin llamadas bloqueantes en los callbacks de LVGL) e indica en el pull request con qué placa lo has probado.
