#include "victron/devices/VictronBatteryMonitor.h"

VictronBatteryMonitor::VictronBatteryMonitor(const std::string &mac_address_hex_colon, const std::string &instant_readout_key_hex)
    : VictronDevice(mac_address_hex_colon, instant_readout_key_hex)
{
}

const victron_device_class_t VictronBatteryMonitor::get_device_class() const
{
    return VICTRON_DEVICE_CLASS_BATTERYMONITOR;
}

VictronBatteryMonitorData *VictronBatteryMonitor::parse_data(const std::string &raw_data)
{
    VictronBatteryMonitorData *data = new VictronBatteryMonitorData(raw_data);
    data->decrypt(key);
    data->parse();
    return data;
}

VictronBatteryMonitor::~VictronBatteryMonitor()
{
}

VictronBatteryMonitorData::VictronBatteryMonitorData()
{
}

VictronBatteryMonitorData::VictronBatteryMonitorData(const VictronData &data) : VictronData(data)
{
}

VictronBatteryMonitorData::VictronBatteryMonitorData(const std::string &raw_data) : VictronData(raw_data)
{
}

void VictronBatteryMonitorData::parse()
{
    memcpy(&data, get_decrypted_c(), sizeof(battery_monitor_data));

    main_voltage = data.voltage_10mv / 100.0f;
    aux_voltage = data.aux_value / 100.0f;
    current = data.current_milliamps / 1000.0f;
    soc = data.soc_per_mille / 10.0f;
}

const battery_monitor_data &VictronBatteryMonitorData::get_battery_monitor_data() const
{
    return data;
}

VictronBatteryMonitorData::~VictronBatteryMonitorData()
{
}