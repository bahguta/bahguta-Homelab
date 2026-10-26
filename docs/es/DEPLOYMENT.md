# Despliegue en el servidor

Cómo se organizan los ficheros en el servidor y cómo desplegar, a mano o con el pipeline de GitLab. El [README principal](../../README.es.md) tiene la versión corta; esta guía explica las carpetas que espera el proyecto.

## Estructura en el servidor

```
/opt/bahguta/
├── compose/   postgres.yml, mqtt.yml        ← copia del compose/ del repo
├── config/    postgres/.env, mosquitto/     ← copia del config/ del repo
├── data/      postgres/                     ← se crea al arrancar: haz copia de seguridad
├── scripts/   docker/*.sh, system/*.sh      ← copia del scripts/ del repo
├── spring/    Dockerfile, docker-compose.yml, .env, haproxy.cfg.template, bahguta-api.jar
└── web/       docker-compose.yml, .env, htpasswd
```

`/opt/bahguta` lo usan el pipeline y dos rutas absolutas: el backend monta `/opt/bahguta/scripts` y `scripts/system/*.sh` llaman a `/opt/bahguta/compose/postgres.yml`. Si prefieres otra carpeta, cambia esas rutas (y `DEPLOY_DIR` en `.gitlab-ci.yml`).

## Despliegue manual

```bash
# 1. Ficheros
sudo mkdir -p /opt/bahguta && sudo chown "$USER" /opt/bahguta
cd /opt/bahguta
cp -r ~/bahguta-homelab/{compose,config,scripts} .
mkdir -p data/postgres spring web
cp ~/bahguta-homelab/spring/{Dockerfile,docker-compose.yml,.env.example,make-proxy-template.sh} spring/

# 2. PostgreSQL
docker network create web                       # solo la primera vez
cp config/postgres/.env.example config/postgres/.env && nano config/postgres/.env
docker compose -f compose/postgres.yml up -d

# 3. Backend (compila el jar en cualquier máquina con Java 21 y Maven)
cd ~/bahguta-homelab && mvn -f spring/bahguta-api/pom.xml clean package -Dmaven.test.skip=true
cp spring/bahguta-api/target/bahguta-api-*.jar /opt/bahguta/spring/bahguta-api.jar
cd /opt/bahguta/spring
cp .env.example .env && nano .env               # ver «Variables de entorno» en el README
bash make-proxy-template.sh
docker compose up -d --build

# 4. Web (opcional): sigue bahguta_spa/README.es.md, «Despliegue»
```

## Despliegue con el pipeline de GitLab

`.gitlab-ci.yml` tiene dos jobs que se ejecutan al hacer push a `main`: **backend** (cuando cambia `spring/`) y **web** (cuando cambia `bahguta_spa/`).

Requisitos:

- Un runner de GitLab con acceso a Docker, en el servidor, con la etiqueta `bahguta-pro` (cámbiala en el fichero si la tuya es otra).
- **Java 21 y Maven en el runner**: el job del backend compila allí el jar.
- El usuario del runner puede leer y escribir `/opt/bahguta/spring` y `/opt/bahguta/web`.
- Creados antes, una sola vez: `spring/{Dockerfile,docker-compose.yml,.env,haproxy.cfg.template}` y `web/{docker-compose.yml,.env,htpasswd}`. El pipeline no los crea.

Qué hace cada job:

1. **Backend:** compila, copia el jar a `/opt/bahguta/spring/bahguta-api.jar`, guarda la imagen anterior como `spring-bahguta:rollback`, reconstruye, recrea solo el contenedor `bahguta` y espera hasta unos 90 s a que Spring Boot indique en el log que ha arrancado.
2. **Web:** guarda la imagen anterior como `bahguta-web:rollback`, compila la app Angular dentro de Docker, recrea solo `bahguta-web` y comprueba que nginx responde.

Si el contenedor nuevo no arranca, ambos jobs restauran la imagen anterior y fallan. Cada job solo toca su propio contenedor, así que PostgreSQL y el otro servicio siguen en marcha.

## Copias de seguridad

Toda la base de datos está en `/opt/bahguta/data/postgres`. Un volcado portable:

```bash
docker exec bahguta-postgres pg_dump -U bahguta bahguta > bahguta-$(date +%F).sql
```

Restáuralo en una base de datos vacía con `docker exec -i bahguta-postgres psql -U bahguta bahguta < bahguta-AAAA-MM-DD.sql`. Guarda copias fuera del servidor y copia también `spring/.env`, `web/.env` y `web/htpasswd`, que no están en el repositorio.
