#pragma once

#include <sstream>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "Logging.h"
#include "victron/devices/VictronBatteryMonitor.h"

namespace espjoker
{

    class BatteryStatus
    {
    private:
        SemaphoreHandle_t battery_status_lock;

    private:
        float soc = 0;

        float battery_voltage = 0;

        float starter_voltage = 0;

        float power = 0;

        int64_t update_timestamp = 0;

    public:
        BatteryStatus();

        void update(VictronBatteryMonitorData* data);

        std::string get_as_short_string() const;

        virtual ~BatteryStatus();
    };
}