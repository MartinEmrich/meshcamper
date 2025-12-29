#include "VictronData.h"

#include <mbedtls/aes.h>

VictronData::VictronData()
{
}

VictronData::VictronData(const std::string &raw_data)
{
    const char *raw_data_c = raw_data.c_str();

    LOG_DEBUG("raw: ");
    for (auto &&c : raw_data)
    {
        LOG_DEBUG("%02x ", c);
    }
    LOG_DEBUG("\n");

    prefix = (raw_data_c[1] << 8) + raw_data_c[0];
    model_id = (raw_data_c[3] << 8) + raw_data_c[2];
    readout_type = raw_data_c[4];
    iv = (raw_data_c[6] << 8) + raw_data_c[5];
    encrypted_data = raw_data.substr(7, -1);
}

/* Decrypt Victron data */
void VictronData::decrypt(const VictronInstantReadoutKey &instant_readout_key)
{
    if (instant_readout_key.get()[0] != encrypted_data[0])
    {
        LOG_ERROR("Instant Readout Key %02x and data init key %02x not matching.", instant_readout_key.get()[0], encrypted_data[0]);
        throw VictronError("First key and data byte mismatch.");
    }

    // There seems to be a pad-to-multiple-of-16-thing going on, let's do 64, should suffice for now.
    // remove first byte (key check)
    uint16_t size = encrypted_data.size() - 1;
    uint8_t decrypted_bytes[64] = {0};
    uint8_t encrypted_bytes[64] = {0};
    assert(size < 64);
    memcpy((void*)encrypted_bytes, encrypted_data.c_str() + 1, size);

    mbedtls_aes_context aes;

    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, instant_readout_key.get(), 128);

    uint8_t nonce_counter[16] = {0}; // "initial value" uses only 2 of possible 16 bytes.
    nonce_counter[0] = iv & 0xFF;
    nonce_counter[1] = (iv % 0xFF00) >> 8;

    uint8_t stream_block[16] = {0};

    size_t nonce_offset = 0;

    mbedtls_aes_crypt_ctr(
        &aes, 
        size, 
        &nonce_offset,
        nonce_counter, 
        stream_block, 
        encrypted_bytes, 
        decrypted_bytes);

    decrypted_data = std::string((const char *)decrypted_bytes, size);
}

const unsigned char *VictronData::get_decrypted_c()
{
    if (decrypted_data.size() == 0)
    {
        throw VictronError("Not decrypted yet.");
    }
    return (const unsigned char *)decrypted_data.c_str();
}

const std::string VictronData::get_decrypted() const
{
    if (decrypted_data.size() == 0)
    {
        throw VictronError("Not decrypted yet.");
    }
    return decrypted_data;
}

const uint16_t VictronData::get_model_id() const
{
    return model_id;
}

const uint8_t VictronData::get_readout_type() const
{
    return readout_type;
}

VictronData::~VictronData()
{
}
