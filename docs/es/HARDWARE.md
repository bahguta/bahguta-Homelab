# Hardware

## Tablet (ESP32-P4)

- Placa **ESP32-P4** con pantalla táctil de **800x1280**.
- Controlador táctil **GSL3680** por I2C.
- Core de Arduino para ESP32 **v3.1**.

Pines usados (`bahguta-display/bahguta_display/pins_config.h`):

| Función | GPIO |
|---|---|
| Reset de la pantalla (`LCD_RST`) | 27 |
| Retroiluminación (`LCD_LED`, PWM a 20 kHz) | 23 |
| Táctil SDA (`TP_I2C_SDA`) | 7 |
| Táctil SCL (`TP_I2C_SCL`) | 8 |
| Táctil reset (`TP_RST`) | 22 |
| Táctil interrupción (`TP_INT`) | 21 |

Los pines pueden ser distintos en otra placa con ESP32-P4: ajústalos en `pins_config.h`.

Notas:

- La retroiluminación se controla por PWM para hacer fade al apagar y encender la pantalla.
- Los iconos y animaciones del tiempo viven en `bahguta_display/data/` y se suben al sistema de ficheros **LittleFS**.

## Sensores (ESP32-C3 + DHT22)

- Placa **ESP32-C3 SuperMini**.
- Sensor **DHT22** (temperatura y humedad) con la librería *DHT sensor library* de Adafruit.

Conexión:

| DHT22 | ESP32-C3 SuperMini |
|---|---|
| VCC | 3V3 |
| GND | GND |
| DATA | **GPIO 4** (con una resistencia pull-up de 10 kΩ a 3V3 si tu módulo no la lleva) |

El pin se define en `src/sensors/TemperatureSensor.h` (`DHT(4, DHT22)`).

### Funcionamiento

1. El nodo despierta, se conecta al WiFi y se registra en el backend.
2. Pide su configuración, mide y envía la medición por HTTP.
3. Entra en *deep sleep* durante `heartbeatSeconds` segundos (30 por defecto, configurable desde el backend) y repite.

Dormir entre mediciones es lo que permite que funcionen a pilas durante mucho tiempo.

### Alimentación con pila 18650

Cada nodo funciona con una pila 18650 y una placa cargadora. El problema habitual: muchas placas y power banks **apagan la salida** cuando el consumo baja de unos 30 mA durante unos 10 segundos, y el ESP32 en *deep sleep* consume muchísimo menos que eso. El nodo se quedaría sin corriente a mitad de ciclo.

Solución usada en este proyecto: una placa *Battery Shield V3* para una pila 18650 con un interruptor **NORMAL / HOLD**. En posición **HOLD** la salida se mantiene encendida hasta agotar la pila, aunque el consumo sea mínimo.

> Algunas unidades de este tipo de placas llegan con soldaduras defectuosas: comprueba cada una antes de montarla.

### Lista de materiales por nodo

- 1 × ESP32-C3 SuperMini
- 1 × sensor DHT22 (y resistencia pull-up de 10 kΩ si hace falta)
- 1 × pila 18650
- 1 × placa Battery Shield V3 (con interruptor NORMAL/HOLD)
- Cables y caja

## Servidor

Cualquier máquina Linux con Docker y unos 2 GB de RAM libres para el backend y PostgreSQL. En el proyecto original funciona en un mini PC.
