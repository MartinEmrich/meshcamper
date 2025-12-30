#pragma once

#include "Logging.h"
#include "MeshtasticClient.h"
#include "BatteryStatus.h"

#include <string>
#include <vector>

// Send report every 2 minutes.
#define DEFAULT_REPORT_INTERVAL_SECONDS 120

// mark reports from battery_status that are older than this
#define REPORT_STALE_THRESHOLD_US 60000000

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

        /* optional, regular short status report */
        bool report_enabled = false;
        uint64_t report_interval_seconds = DEFAULT_REPORT_INTERVAL_SECONDS;
        int64_t last_report_sent_time = 0;

        std::string handle_battery_command(const std::vector<std::string> tokens);

        std::string handle_heater_command(const std::vector<std::string> token);
        
        std::string handle_report_command(const std::vector<std::string> token);

        void send_report();

    public:
        ESPJokerApplication(BatteryStatus *battery_status, MeshtasticClient *meshtastic_client);

        virtual void handle_message(const uint32_t &from, const uint32_t &to, const uint8_t channel, const std::string &message);

        void loop();

        virtual ~ESPJokerApplication();
    };
}