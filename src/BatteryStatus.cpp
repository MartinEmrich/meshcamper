#include "BatteryStatus.h"

namespace espjoker
{

    BatteryStatus::BatteryStatus()
    {
        battery_status_lock = xSemaphoreCreateMutex();
    }

    void BatteryStatus::update(VictronBatteryMonitorData *data)
    {
        xSemaphoreTake(battery_status_lock, portMAX_DELAY);
        battery_voltage = data->main_voltage;
        starter_voltage = data->aux_voltage;
        power = data->main_voltage * data->current;
        soc = data->soc;
        update_timestamp = esp_timer_get_time();
        xSemaphoreGive(battery_status_lock);
    }

    std::string BatteryStatus::get_as_short_string() const
    {
        xSemaphoreTake(battery_status_lock, portMAX_DELAY);
        char buf[100] = {0};
        snprintf(buf, 100, "B:%2.2fV:%.1f%%,S:%2.2fV,P:%.1fW(t-%.1fs)",
                 battery_voltage, soc, starter_voltage, power, (esp_timer_get_time() - update_timestamp) / 1000000.0f);
        xSemaphoreGive(battery_status_lock);
        return std::string(buf);
    }

    const bool BatteryStatus::is_ready() const
    {
        return (battery_voltage != 0.0f || starter_voltage != 0.0f);
    }

    BatteryStatus::~BatteryStatus()
    {
    }
}