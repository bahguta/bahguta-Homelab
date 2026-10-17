#pragma once

#include "../../models/NetworkInfo.h"

class NetworkService
{
public:

    void init();

    void update();

    void reconnect();

    const NetworkInfo& network() const;

private:

    void connect();

    void refresh();

    NetworkInfo currentNetwork;

    uint32_t lastAttempt = 0;

    uint32_t disconnectedSince = 0;

    static constexpr uint32_t RETRY_TIME = 10000;

    static constexpr uint32_t STUCK_REBOOT_TIMEOUT = 5UL * 60UL * 1000UL; // 5 minutos


    
};