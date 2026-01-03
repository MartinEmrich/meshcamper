#pragma once

#include "Logging.h"
#include "config.h"
#include <Arduino.h>
#include "Meshtastic.h"
#include <string>
#include <set>

#define SERIAL_RX 16
#define SERIAL_TX 17

#define BAUDRATE 9600

namespace meshcamper
{
    extern "C"
    {
        void meshcamper_mt_connected_cb(mt_node_t *node, mt_nr_progress_t progress);
        void meshcamper_mt_text_cb(uint32_t from, uint32_t to, uint8_t channel, const char *text);
        void meshcamper_mt_task(void *ptr);
    }

    class MessageHandler
    {
    public:
        virtual void handle_message(const uint32_t &from, const uint32_t &to, const uint8_t channel, const std::string &message) = 0;

        virtual ~MessageHandler();
    };

    class MeshtasticClient
    {
    private:
        /* Sending is always done in the MeshtasticClient task. */
        class SendQueueElement
        {
        public:
            uint32_t to;
            uint8_t ch;
            std::string *msg;

            SendQueueElement(const uint32_t &to, const uint8_t &ch, const std::string &msg);

            ~SendQueueElement();
        };

        static MeshtasticClient *instance;

        std::set<uint32_t> trusted_nodes;

        /* Array of device numbers allowed to contact us.
         * TODO: Use favourited nodes from node_report for that!
         */
        static const uint32_t own_devices[];

        // Our MT address/node num, learned from node report.
        uint32_t own_address = 0;

        unsigned long last_nr_time = 0;

        unsigned long last_device_contact = 0;

        QueueHandle_t send_q = NULL;

        MessageHandler *message_handler = NULL;

        bool can_send = false;
        bool connected = false;

        /* freertos task glue function */
        friend void meshcamper_mt_task(void *ptr);
        friend void meshcamper_mt_connected_cb(mt_node_t *node, mt_nr_progress_t progress);
        friend void meshcamper_mt_text_cb(uint32_t from, uint32_t to, uint8_t channel, const char *text);

        uint64_t last_device_contact_time();

        /* (re)initialize meshtastic serial connection */
        void init();

        /* FreeRTOS task interacting with MT */
        void task();

        /* Node report callback */
        void node_report(mt_node_t *node, mt_nr_progress_t progress);

        /* Text Message callback */
        void handle_message(uint32_t from, uint32_t to, uint8_t channel, const char *text);

    public:
        MeshtasticClient();

        void start();

        /** Send (enqueue) message
         * Send a Text Message to a recipient (or broadcast)
         *
         * Message is enqueued for sending, not sent synchronously.
         */
        void send(const std::string &message, uint32_t to = BROADCAST_ADDR);

        /* Register message handler.
         * (replaces a previously registered one)
         */
        void register_handler(MessageHandler *handler);

        virtual ~MeshtasticClient();
    };
}