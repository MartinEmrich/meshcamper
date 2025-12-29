#pragma once

#include "victron/VictronData.h"
#include "victron/VictronDevice.h"
#include "stdint.h"
#include "Logging.h"

typedef struct battery_monitor_data
{
    uint16_t time_remaining_min : 16;
    int16_t voltage_10mv : 16;
    uint16_t alarm : 16;
    uint16_t aux_value : 16;
    uint8_t aux_mode : 2;
    int32_t current_milliamps : 22;
    uint32_t consumed_tenth_amphours : 20;
    uint16_t soc_per_mille : 10;
} __attribute__((packed)) battery_monitor_data_t;

class VictronBatteryMonitorData : public VictronData

{
private:
    // raw
    battery_monitor_data data;

public:
    float main_voltage;

    float aux_voltage;

    float soc;

    float current;

    VictronBatteryMonitorData();

    VictronBatteryMonitorData(const VictronData &data);

    VictronBatteryMonitorData(const std::string &raw_data);

    virtual void parse();

    const battery_monitor_data &get_battery_monitor_data() const;

    virtual ~VictronBatteryMonitorData();
};

class VictronBatteryMonitor : public VictronDevice
{
public:
    VictronBatteryMonitor(const std::string &mac_address_hex_colon, const std::string &instant_readout_key_hex);

    /** Parse raw BLE data string (minus the leading 2-byte vendor ID)
     *
     * Returns parsed and decrypted instant readout data.
     */
    virtual VictronBatteryMonitorData *parse_data(const std::string &raw_data);

    virtual const victron_device_class_t get_device_class() const;

    virtual ~VictronBatteryMonitor();
};
