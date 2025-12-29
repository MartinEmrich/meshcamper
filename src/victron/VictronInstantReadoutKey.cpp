#include "VictronInstantReadoutKey.h"

VictronInstantReadoutKey::VictronInstantReadoutKey(const std::string &hex)
{
    if (hex.size() != 32)
    {
        throw VictronError("Invalid key length. must be 32 hex chars.");
    }
    for (unsigned int i = 0; i < 16; i++)
    {
        std::string byte = hex.substr(i * 2, 2);
        uint8_t c;
        sscanf(byte.c_str(), "%hhx", &c);
        // LOG_DEBUG("Char %d is %s converted to 0x%02x\n", i, byte.c_str(), c);
        key[i] = c;
    }
}
const unsigned char *VictronInstantReadoutKey::get() const
{
    return key;
}

VictronInstantReadoutKey::~VictronInstantReadoutKey()
{
}
