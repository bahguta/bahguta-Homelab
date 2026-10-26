# Server deployment

How the files are laid out on the server and how to deploy, by hand or with the GitLab pipeline. The [main README](../README.md) has the short version; this guide explains the folders the project expects.

## Layout on the server

```
/opt/bahguta/
├── compose/   postgres.yml, mqtt.yml        ← copy of the repo's compose/
├── config/    postgres/.env, mosquitto/     ← copy of the repo's config/
├── data/      postgres/                     ← created on first start: back it up
├── scripts/   docker/*.sh, system/*.sh      ← copy of the repo's scripts/
├── spring/    Dockerfile, docker-compose.yml, .env, haproxy.cfg.template, bahguta-api.jar
└── web/       docker-compose.yml, .env, htpasswd
```

`/opt/bahguta` is used by the pipeline and by two absolute paths: the backend mounts `/opt/bahguta/scripts` and `scripts/system/*.sh` call `/opt/bahguta/compose/postgres.yml`. If you prefer another folder, change those paths (and `DEPLOY_DIR` in `.gitlab-ci.yml`).

## Manual deployment

```bash
# 1. Files
sudo mkdir -p /opt/bahguta && sudo chown "$USER" /opt/bahguta
cd /opt/bahguta
cp -r ~/bahguta-homelab/{compose,config,scripts} .
mkdir -p data/postgres spring web
cp ~/bahguta-homelab/spring/{Dockerfile,docker-compose.yml,.env.example,make-proxy-template.sh} spring/

# 2. PostgreSQL
docker network create web                       # first time only
cp config/postgres/.env.example config/postgres/.env && nano config/postgres/.env
docker compose -f compose/postgres.yml up -d

# 3. Backend (build the jar on any machine with Java 21 and Maven)
cd ~/bahguta-homelab && mvn -f spring/bahguta-api/pom.xml clean package -Dmaven.test.skip=true
cp spring/bahguta-api/target/bahguta-api-*.jar /opt/bahguta/spring/bahguta-api.jar
cd /opt/bahguta/spring
cp .env.example .env && nano .env               # see "Environment variables" in the README
bash make-proxy-template.sh
docker compose up -d --build

# 4. Web app (optional): follow bahguta_spa/README.md, "Deployment"
```

## Deployment with the GitLab pipeline

`.gitlab-ci.yml` has two jobs that run on pushes to `main`: **backend** (when `spring/` changes) and **web** (when `bahguta_spa/` changes).

Requirements:

- A GitLab runner with Docker access, on the server, with the tag `bahguta-pro` (change it in the file if yours is different).
- **Java 21 and Maven on the runner**: the backend job compiles the jar there.
- The runner's user can read and write `/opt/bahguta/spring` and `/opt/bahguta/web`.
- Created beforehand, once: `spring/{Dockerfile,docker-compose.yml,.env,haproxy.cfg.template}` and `web/{docker-compose.yml,.env,htpasswd}`. The pipeline does not create them.

What each job does:

1. **Backend:** compiles, copies the jar to `/opt/bahguta/spring/bahguta-api.jar`, keeps the previous image as `spring-bahguta:rollback`, rebuilds, recreates only the `bahguta` container and waits up to about 90 s for Spring Boot to log that it has started.
2. **Web:** keeps the previous image as `bahguta-web:rollback`, builds the Angular app inside Docker, recreates only `bahguta-web` and checks that nginx answers.

If the new container does not start, both jobs put the previous image back and fail. The jobs only touch their own container, so PostgreSQL and the other service stay up.

## Backups

The whole database is in `/opt/bahguta/data/postgres`. A portable dump:

```bash
docker exec bahguta-postgres pg_dump -U bahguta bahguta > bahguta-$(date +%F).sql
```

Restore it into an empty database with `docker exec -i bahguta-postgres psql -U bahguta bahguta < bahguta-YYYY-MM-DD.sql`. Keep copies off the server, and also copy `spring/.env`, `web/.env` and `web/htpasswd`, which are not in the repository.
