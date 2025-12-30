#pragma once

#include "Logging.h"
#include "config.h"
#include <Arduino.h>
#include "Meshtastic.h"
#include <string>  

#define SERIAL_RX 16
#define SERIAL_TX 17

#define BAUDRATE 9600

namespace espjoker
{
    extern "C"
    {
        void espjoker_mt_connected_cb(mt_node_t *node, mt_nr_progress_t progress);
        void espjoker_mt_text_cb(uint32_t from, uint32_t to, uint8_t channel, const char *text);

        void espjoker_mt_task(void *ptr);
    }

    class MeshtasticClient
    {
    private:
        static MeshtasticClient *instance;
        bool can_send = false;
        bool connected = false;

        /* freertos task glue function */
        friend void espjoker_mt_task(void *ptr);
        friend void espjoker_mt_connected_cb(mt_node_t *node, mt_nr_progress_t progress);
        friend void espjoker_mt_text_cb(uint32_t from, uint32_t to, uint8_t channel, const char *text);

        void task();

        void node_report(mt_node_t *node, mt_nr_progress_t progress);

    public:
        MeshtasticClient();

        void start();

        void send(const std::string& message, uint32_t recipient = MESHTASTIC_DEFAULT_RECIPIENT);

        virtual ~MeshtasticClient();
    };
}