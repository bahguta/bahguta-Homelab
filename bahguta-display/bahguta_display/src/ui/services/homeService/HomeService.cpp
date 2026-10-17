#include "HomeService.h"

#include "../ServiceManager.h"

void HomeService::init()
{
}


void HomeService::update()
{
    // Pequenos y cambian por su cuenta (WiFi, CPU tablet): siempre
    current.network =
        ServiceManager::network().network();

    current.system =
        ServiceManager::system().system();

    // Lo que viene del backend: solo cuando llego un dashboard nuevo
    // (antes se copiaba entero cada 10 ms dentro del mutex)
    const uint32_t v = ServiceManager::api().version();

    if (v == seenVersion)
        return;

    seenVersion = v;

    current.weather =
        ServiceManager::api().weather();

    current.devices =
        ServiceManager::api().devices();

    current.sensors =
        ServiceManager::api().sensors();

    current.server =
        ServiceManager::api().server();

    current.red =
        ServiceManager::api().red();

    current.grafana =
        ServiceManager::api().grafana();

    const DockerInfo& docker =
        ServiceManager::api().docker();

    current.docker.online =
        docker.online;

    current.docker.total =
        docker.total;

    current.docker.connected =
        docker.connected;
}

const HomeInfo& HomeService::home() const
{
    return current;
}