#include "DockerService.h"
#include "../../models/DockerInfo.h"
#include "../ServiceManager.h"

void DockerService::init()
{

}

void DockerService::update()
{
    const uint32_t v = ServiceManager::api().version();

    if (v == seenVersion)
        return;

    seenVersion = v;

    dockerInfo = ServiceManager::api().docker();
}


DockerInfo& DockerService::docker()
{
    return ServiceManager::api().docker();
}