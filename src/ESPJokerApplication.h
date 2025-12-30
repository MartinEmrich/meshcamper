#pragma once

#include "Logging.h"
#include "MeshtasticClient.h"
#include "BatteryStatus.h"

#include <string>
#include <vector>

namespace espjoker
{
    /* ESP Joker - ESP32 and Meshtastic for Club Joker (City)
     *
     * Main state machine and application */

    class ESPJokerApplication : public MessageHandler
    {
    private:
        std::vector<std::string> tokenize(const std::string &message);

        BatteryStatus *battery_status;

        MeshtasticClient *meshtastic_client;

        std::string handle_battery_command(const std::vector<std::string> tokens);

    public:
        ESPJokerApplication(BatteryStatus *battery_status, MeshtasticClient *meshtastic_client);

        virtual void handle_message(const uint32_t &from, const uint32_t &to, const uint8_t channel, const std::string &message);

        virtual ~ESPJokerApplication();
    };
}