#pragma once

#include "Logging.h"
#include "VictronInstantReadoutKey.h"
#include "VictronData.h"
#include <NimBLEAddress.h>
#include <string>

typedef enum victron_device_class
{
    VICTRON_DEVICE_CLASS_BATTERYMONITOR
    // There are more, but that's all here for now;
} victron_device_class_t;

class VictronDevice
{
protected:
    NimBLEAddress address;
    VictronInstantReadoutKey key;

public:
    VictronDevice(const std::string &mac_address_hex_colon, const std::string &instant_readout_key_hex);

    /* Test if this is the device. Remember: Victron device addresses seem to be RANDOM, not PUBLIC */
    const bool matches(const NimBLEAddress &test_address) const;

    /* Get parsed and decrypted device-class-specific data object.
     * delete after use!
     */
    virtual VictronData *parse_data(const std::string &raw_data) = 0;

    virtual const victron_device_class_t get_device_class() const = 0;

    virtual ~VictronDevice();
};