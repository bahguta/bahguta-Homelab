# bahguta display (ESP32-P4 tablet)

🇬🇧 English · 🇪🇸 [Español](README.es.md)

Firmware for the touch tablet: an Arduino sketch with an LVGL interface that polls the backend's dashboard and draws the screens (home, house, weather, Docker, devices, network, system and settings).

## Hardware

- ESP32-P4 board with an **800x1280 display** (JD9365 driver, in `src/lcd/`) and a **GSL3680 touch controller** (`src/touch/`). Pins are in `pins_config.h`.
- **16 MB of flash**: `partitions.csv` defines two OTA application slots and an 8 MB LittleFS partition (labelled `spiffs`).

## Requirements

- Arduino IDE or `arduino-cli` with the **Arduino core for ESP32 v3.1**. Other versions are not supported.
- The **LVGL 9.2** library for Arduino (this folder's `lv_conf.h` is written for v9.2.2). Copy this folder's `lv_conf.h` to the Arduino `libraries/` folder, next to `lvgl/`, as LVGL's Arduino instructions require.
- A tool to upload a folder to LittleFS from the Arduino IDE.

## Build and flash

1. Copy `src/ui/Secrets.example.h` to `src/ui/Secrets.h` and set `BAHGUTA_API_KEY` to the same value as the backend's `.env`. `Secrets.h` is in `.gitignore`: never commit it. Without the key, starting and stopping containers fails with `401`.
2. Open `bahguta_display.ino`, select your ESP32-P4 board and the **custom partition table** (`partitions.csv` in this folder), then build and upload.
3. Upload the `data/` folder (`icons`, `weather`, `weather-mini`, `weather-slot`) to the **LittleFS** partition.
4. On first boot open **Settings** and enter the WiFi and the backend URL (for example `http://bahguta.local`). They are stored on the tablet, not in the code.

## Structure

```
bahguta_display.ino   entry point: display, touch, LittleFS and UI start-up
src/lcd, src/touch    display and touch drivers
src/ui/layout         main layout: header, footer, bottom menu and body views
src/ui/screens        one file per screen
src/ui/services       backend access, settings, time and the rest of the services
src/ui/widgets        reusable components (cards, switches...)
src/ui/themes         colours and styles
data/                 weather icons and animations (uploaded to LittleFS)
```

Contributing: keep the code responsive (no blocking calls in LVGL callbacks) and say in your pull request which board you tested on.
