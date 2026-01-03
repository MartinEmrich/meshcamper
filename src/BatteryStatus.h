#pragma once

#include <sstream>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "Logging.h"
#include "victron/devices/VictronBatteryMonitor.h"

namespace meshcamper
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

        const bool is_ready() const;

        const float get_soc() const;

        const float get_power() const;

        const float get_battery_voltage() const;

        const float get_starter_voltage() const;

        const uint32_t get_age_ms() const;

        virtual ~BatteryStatus();
    };
}