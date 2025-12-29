#pragma once

#include "Logging.h"
#include "VictronError.h"

#include <stdint.h>
#include <string>

class VictronInstantReadoutKey
{
private:
    uint8_t key[16] = {0};

public:
    VictronInstantReadoutKey(const std::string &hex);

    const unsigned char *get() const;

    virtual ~VictronInstantReadoutKey();
};
