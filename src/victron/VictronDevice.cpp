#include "VictronDevice.h"

VictronDevice::VictronDevice(
    const std::string& mac_address_hex_colon, 
    const std::string& instant_readout_key_hex) :
    address(mac_address_hex_colon, BLE_ADDR_RANDOM),
    key(instant_readout_key_hex)
{
    
} 

const bool VictronDevice::matches(const NimBLEAddress& test_address) const {
    return (address == test_address);
}


VictronDevice::~VictronDevice() {

}
