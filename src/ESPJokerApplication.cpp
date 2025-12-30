#include "ESPJokerApplication.h"

namespace espjoker
{

    ESPJokerApplication::ESPJokerApplication(BatteryStatus *_battery_status, MeshtasticClient *_meshtastic_client) : battery_status(_battery_status), meshtastic_client(_meshtastic_client)
    {
        meshtastic_client->register_handler(this);
    }

    std::vector<std::string>
    ESPJokerApplication::tokenize(const std::string &message)
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

    void ESPJokerApplication::handle_message(const uint32_t &from, const uint32_t &to, const uint8_t channel, const std::string &message)
    {
        if (to == BROADCAST_ADDR && channel != 0) {
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

    std::string ESPJokerApplication::handle_battery_command(const std::vector<std::string> tokens)
    {
        return battery_status->get_as_short_string();
    }

    ESPJokerApplication::~ESPJokerApplication()
    {
    }
}