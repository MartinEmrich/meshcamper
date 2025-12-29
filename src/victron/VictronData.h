#pragma once
#include <string>
#include "Logging.h"
#include "victron/VictronError.h"
#include "victron/VictronInstantReadoutKey.h"

class VictronData
{
private:
    uint16_t prefix;
    uint16_t model_id;
    uint8_t readout_type;
    uint16_t iv;
    std::string encrypted_data;
    std::string decrypted_data;

public:
    VictronData();

    VictronData(const std::string &raw_data);

    virtual void decrypt(const VictronInstantReadoutKey &key);

    virtual const unsigned char *get_decrypted_c();

    virtual const std::string get_decrypted() const;

    virtual const uint16_t get_model_id() const;

    virtual const uint8_t get_readout_type() const;

    virtual ~VictronData();
};
