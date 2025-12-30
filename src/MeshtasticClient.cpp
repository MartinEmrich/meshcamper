#include "MeshtasticClient.h"

namespace espjoker
{
    TaskHandle_t mt_task_handle;

    MeshtasticClient* MeshtasticClient::instance = NULL;

    extern "C"
    {
        void espjoker_mt_connected_cb(mt_node_t *node, mt_nr_progress_t progress)
        {
            LOG_INFO("MT connected, progress: %d\n", progress);
            MeshtasticClient::instance->node_report(node, progress);
        }

        void espjoker_mt_text_cb(uint32_t from, uint32_t to, uint8_t channel, const char *text)
        {
            LOG_INFO("MMMMMMMMMMMMMMMMMMMMMMMMMMM > Got text from 0x%08x: %s\n\n", from, text);
        }

        void espjoker_mt_task(void *ptr)
        {
            static_cast<espjoker::MeshtasticClient *>(ptr)->task();
        }
    }

    MeshtasticClient::MeshtasticClient() :
    can_send(false),
    connected(false)
    {
        instance = this;
        mt_serial_init(SERIAL_RX, SERIAL_TX, BAUDRATE);
        bool rv = mt_request_node_report(espjoker_mt_connected_cb);
        LOG_INFO("Node report result: %d\n", rv);
        set_text_message_callback(espjoker_mt_text_cb);
        LOG_INFO("Meshtastic initialized.");
    }

    void MeshtasticClient::start()
    {
        LOG_INFO("Starting MT Task\n");
        xTaskCreate(espjoker_mt_task, "Meshtastic Task", 4096, this, 1, &mt_task_handle);
    }

    void MeshtasticClient::task()
    {
        LOG_INFO("Meshtastic task started.\n");
        while (true)
        {
            can_send = mt_loop(millis());
            if (!can_send)
            {
                LOG_ERROR("MT connection not ready to send!");
            }
            // TODO: How much for MT?
            delay(50);
            // LOG_DEBUG("MT Task mem watermark: %d bytes. Scanning again in 5s\n", uxTaskGetStackHighWaterMark(mt_task_handle));
        }
    }

    void MeshtasticClient::node_report(mt_node_t *node, mt_nr_progress_t progress) {
        LOG_INFO("Got node report, assuming meshtastic device is up!\n");
        connected = true;
    }

    void MeshtasticClient::send(const std::string& message, uint32_t recipient) {
        mt_send_text(message.c_str(), recipient, 0 /* TODO */);
    }


    MeshtasticClient::~MeshtasticClient()
    {
    }

}