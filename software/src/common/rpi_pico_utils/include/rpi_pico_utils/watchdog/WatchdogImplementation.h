#pragma once

#include "WatchdogInterface.h"

#include <hardware/watchdog.h>
#include <config/common/watchdog/Config.h>

class WatchdogImplementation : public WatchdogInterface
{
public:
    WatchdogImplementation()
    {
        watchdog_enable(config::common::watchdog::TIMEOUT_MS, true);
    }

    virtual void update() override
    {
        watchdog_update();
    }
};