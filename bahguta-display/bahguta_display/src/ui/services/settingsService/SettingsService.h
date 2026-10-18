#pragma once

#include "../../models/SettingsInfo.h"

class SettingsService
{
public:

    void init();

    bool load();
    bool save();

    SettingsInfo& settings();

private:

    SettingsInfo currentSettings;
};