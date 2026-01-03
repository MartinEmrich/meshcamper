#include "BatteryStatus.h"

namespace meshcamper
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

    bool BatteryStatus::is_ready() const
    {
        return (battery_voltage != 0.0f || starter_voltage != 0.0f);
    }

    float BatteryStatus::get_soc() const
    {
        return soc;
    }

    float BatteryStatus::get_power() const
    {
        return power;
    }

    float BatteryStatus::get_battery_voltage() const
    {
        return battery_voltage;
    }

    float BatteryStatus::get_starter_voltage() const
    {
        return starter_voltage;
    }

    uint32_t BatteryStatus::get_age_ms() const
    {
        return (esp_timer_get_time() - update_timestamp);
    }

    BatteryStatus::~BatteryStatus()
    {
    }
}