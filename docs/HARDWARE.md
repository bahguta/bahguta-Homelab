# Hardware

## Tablet (ESP32-P4)

- **ESP32-P4** board with an **800x1280** touch display.
- **GSL3680** touch controller over I2C.
- Arduino core for ESP32 **v3.1**.

Pins used (`bahguta-display/bahguta_display/pins_config.h`):

| Function | GPIO |
|---|---|
| Display reset (`LCD_RST`) | 27 |
| Backlight (`LCD_LED`, 20 kHz PWM) | 23 |
| Touch SDA (`TP_I2C_SDA`) | 7 |
| Touch SCL (`TP_I2C_SCL`) | 8 |
| Touch reset (`TP_RST`) | 22 |
| Touch interrupt (`TP_INT`) | 21 |

Pins may differ on another ESP32-P4 board: adjust them in `pins_config.h`.

Notes:

- The backlight is driven with PWM so it can fade out and in when the screen sleeps and wakes.
- The weather icons and animations live in `bahguta_display/data/` and are uploaded to the **LittleFS** filesystem.

## Sensors (ESP32-C3 + DHT22)

- **ESP32-C3 SuperMini** board.
- **DHT22** sensor (temperature and humidity) using Adafruit's *DHT sensor library*.

Wiring:

| DHT22 | ESP32-C3 SuperMini |
|---|---|
| VCC | 3V3 |
| GND | GND |
| DATA | **GPIO 4** (with a 10 kΩ pull-up resistor to 3V3 if your module does not include one) |

The pin is defined in `src/sensors/TemperatureSensor.h` (`DHT(4, DHT22)`).

### How it works

1. The node wakes up, connects to WiFi and registers with the backend.
2. It requests its configuration, measures and sends the measurement over HTTP.
3. It enters *deep sleep* for `heartbeatSeconds` seconds (30 by default, configurable from the backend) and repeats.

Sleeping between measurements is what lets the nodes run on batteries for a long time.

### Powering from an 18650 cell

Each node runs from a single 18650 cell and a charger board. The usual problem: many boards and power banks **shut off their output** when the load stays below about 30 mA for around 10 seconds, and an ESP32 in *deep sleep* draws far less than that. The node would lose power mid-cycle.

Solution used in this project: a single-cell *Battery Shield V3* board with a **NORMAL / HOLD** switch. In **HOLD** the output stays on until the battery is empty, even at very low current.

> Some units of this kind of board arrive with defective soldering: check each one before building the node.

### Bill of materials per node

- 1 × ESP32-C3 SuperMini
- 1 × DHT22 sensor (and a 10 kΩ pull-up resistor if needed)
- 1 × 18650 cell
- 1 × Battery Shield V3 board (with NORMAL/HOLD switch)
- Wires and an enclosure

## Server

Any Linux machine with Docker and about 2 GB of free RAM for the backend and PostgreSQL. The original project runs on a mini PC.
