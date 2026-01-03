#include "MeshcamperApplication.h"

namespace meshcamper
{

    MeshcamperApplication::MeshcamperApplication(BatteryStatus *_battery_status, MeshtasticClient *_meshtastic_client) : battery_status(_battery_status), meshtastic_client(_meshtastic_client)
    {
        meshtastic_client->register_handler(this);
        display = Display::create();
    }

    std::vector<std::string>
    MeshcamperApplication::tokenize(const std::string &message)
    {
        std::vector<std::string> tokens;

        std::string token = "";

        for (auto &&c : message)
        {
            if (c == ' ')
            {

                if (token.size() > 0)
                {
                    tokens.push_back(token);
                    token = "";
                }
            }
            else
            {
                token.push_back(std::tolower(c));
            }
        }
        if (token.size() > 0)
        {
            tokens.push_back(token);
            token = "";
        }

        return tokens;
    }

    void MeshcamperApplication::handle_message(const uint32_t &from, const uint32_t &to, const uint8_t channel, const std::string &message)
    {
        if (to == BROADCAST_ADDR && channel != 0)
        {
            LOG_INFO("Ignoring broadcast channel message to channel %d\n", channel);
            return;
        }

        auto tokens = tokenize(message);

        if (tokens.size() < 2)
        {
            return;
        }
        if (tokens[0] != COMMAND_WORD)
        {
            return;
        }
        if (tokens[1].size() == 0)
        {
            return;
        }

        tokens.erase(tokens.begin());
        std::string command = tokens[0];
        tokens.erase(tokens.begin());

        std::string reply = "";

        switch (command[0])
        {
        case 'b': // Battery
            reply = handle_battery_command(tokens);
            break;
        case 'h': // Heater
            reply = handle_heater_command(tokens);
            break;
        case 'r': // Report
            reply = handle_report_command(tokens);
            break;

        default:
            reply = "Unknown command";
            break;
        }

        // let echoes of incoming message pass...
        delay(500);
        if (reply.size() > 0)
        {
#ifdef REPLY_AS_DM
            // Always reply as DM, to leverage routing/priorities
            meshtastic_client->send(reply, from);
#else
            // Reply as received (DM or primary channel)
            meshtastic_client->send(reply, to == BROADCAST_ADDR ? BROADCAST_ADDR : from);
#endif
        }
    }

    std::string MeshcamperApplication::handle_battery_command(const std::vector<std::string> tokens)
    {
        return battery_status->get_as_short_string();
    }

    std::string MeshcamperApplication::handle_heater_command(const std::vector<std::string> token)
    {
        return "Heater control not yet implemented.";
    }

    std::string MeshcamperApplication::handle_report_command(const std::vector<std::string> token)
    {
        if (token.size() > 0 && token[0].size() > 0)
        {
            switch (token[0][0])
            {
            case '0':
            case 'n':
                report_enabled = false;
                break;
            case '1':
            case 'y':
                report_enabled = true;
                if (token.size() > 1 && token[1].size() > 0)
                {
                    int64_t new_int = std::stoll(token[1]);
                    if (new_int < 5 || new_int > 3600)
                    {
                        return "Interval out of range";
                    }
                    report_interval_seconds = new_int;
                }
                break;
            }
        }
        else // no arg? just toggle
            report_enabled = !report_enabled;

        return std::string(report_enabled ? "Report enabled." : "Report disabled.");
    }

    void MeshcamperApplication::loop()
    {
        if (report_enabled)
        {
            int64_t report_age = esp_timer_get_time() - last_report_sent_time;
            LOG_DEBUG("Report enabled, %d us ago\n", report_age);
            if (report_age > (report_interval_seconds * 1000000))
            {
                LOG_DEBUG("Sending report\n");
                send_report();
                last_report_sent_time = esp_timer_get_time();
            }
        }
        display->update_battery_status(battery_status->get_soc(), battery_status->get_power());
        display->redraw();
        delay(500);
    }

    void MeshcamperApplication::send_report()
    {
        char *report;
        asprintf(&report, "B%.0f%%S%2.1fV%s",
                 battery_status->get_soc(),
                 battery_status->get_starter_voltage(),
                 battery_status->get_age_ms() > REPORT_STALE_THRESHOLD_US ? " STALE!" : "");
        meshtastic_client->send(std::string(report));
        free(report);
    }

    MeshcamperApplication::~MeshcamperApplication()
    {
        delete display;
    }
}