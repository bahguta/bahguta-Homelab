# Hoja de ruta

Ideas abiertas para el proyecto. No son compromisos ni tienen fecha: sirven como punto de partida para quien quiera colaborar. Si te interesa alguna, abre un *issue* para comentarla.

## En el aire

- **Nivel de batería** de cada sensor, medido en el ESP32-C3 y mostrado en la tablet (pantallas de Dispositivos y Casa).
- **Intervalos editables desde la tablet**: intervalo de medición de los sensores y refresco del REST.
- **Unificar Dispositivo y Sensor** en la interfaz y corregir los dispositivos que figuran *online* cuando ya no lo están.

## Seguridad

- Autenticación en el resto de endpoints de la API (hoy solo la exigen los de arrancar y parar Docker); por ejemplo, una clave por dispositivo y otra para la tablet.
- HTTPS entre la tablet y el backend, para que las claves no viajen en claro.
- Que `secret_key` de los dispositivos sea un secreto real y no el nombre de la habitación.
- Autenticación en el broker MQTT si se llega a usar.

## Ampliaciones posibles

- **MQTT:** usar el broker que ya está preparado para publicar mediciones, por ejemplo hacia Home Assistant.
- Más tipos de sensor (BME280, suelo, lluvia, PIR, relés): el esquema de base de datos ya los contempla.
- Más idiomas en la interfaz de la tablet.
- Fotos y capturas en la documentación, y un manual de montaje paso a paso.
- Tests automáticos del backend y una pipeline de compilación del firmware.

## Hecho

- Backend REST con dashboard agregado: una sola petición cada 30 s para toda la tablet.
- Interfaz de la tablet con pantallas de casa, tiempo, dispositivos, sensores, Docker, red, Grafana, sistema y ajustes.
- Splash animado, transiciones suaves entre pantallas y fade del brillo al dormir y despertar.
- Versión del proyecto mostrada en la tablet, tomada del backend.
- Despliegue continuo con GitLab CI y rollback automático.
- Control de Docker reforzado: API key en arrancar/parar, lista blanca de contenedores y un proxy restringido del socket de Docker en lugar de montar `docker.sock`.
