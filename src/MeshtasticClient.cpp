#include "MeshtasticClient.h"

#define DEVICE_CONTACT() last_device_contact = millis();

namespace meshcamper
{
    TaskHandle_t mt_task_handle;

    MeshtasticClient *MeshtasticClient::instance = NULL;

/* @deprecated */
#ifdef OWN_DEVICES
    const uint32_t MeshtasticClient::own_devices[] = OWN_DEVICES;
#endif

    extern "C"
    {
        void meshcamper_mt_connected_cb(mt_node_t *node, mt_nr_progress_t progress)
        {
            MeshtasticClient::instance->node_report(node, progress);
        }

        void meshcamper_mt_text_cb(uint32_t from, uint32_t to, uint8_t channel, const char *text)
        {
            LOG_INFO("MMM <<< Got text from 0x%08x: \"%s\"\n\n", from, text);
            MeshtasticClient::instance->handle_message(from, to, channel, text);
        }

        void meshcamper_mt_task(void *ptr)
        {
            static_cast<meshcamper::MeshtasticClient *>(ptr)->task();
        }
    }

    MessageHandler::~MessageHandler() {}

    MeshtasticClient::MeshtasticClient() : can_send(false),
                                           connected(false)
    {
        instance = this;
        send_q = xQueueCreate(8, sizeof(MeshtasticClient::SendQueueElement *));
/* @deprecated */
#ifdef OWN_DEVICES
        const uint32_t own_devices[] = OWN_DEVICES;
        int i = 0;
        while (own_devices[i] != 0)
        {
            trusted_nodes.insert(own_devices[i++]);
        }
#endif
        init();
    }

    void MeshtasticClient::init()
    {
        connected = false;
        mt_serial_init(SERIAL_RX, SERIAL_TX, BAUDRATE);
        set_text_message_callback(meshcamper_mt_text_cb);
        LOG_INFO("Meshtastic initialized.");
        bool rv = mt_request_node_report(meshcamper_mt_connected_cb);
        LOG_INFO("Initial node report result: %d\n", rv);
        if (rv)
        {
            DEVICE_CONTACT();
        }
    }

    void MeshtasticClient::start()
    {
        LOG_INFO("Starting MT Task\n");
        xTaskCreate(meshcamper_mt_task, "Meshtastic Task", 8192, this, 1, &mt_task_handle);
    }

    void MeshtasticClient::task()
    {
        LOG_INFO("Meshtastic task started.\n");
        while (true)
        {
            //LOG_DEBUG("Last device contact %d ms ago.", last_device_contact_time());
            //LOG_DEBUG("%d trusted nodes", trusted_nodes.size());
            //LOG_DEBUG("Connected: %s.\n", connected ? "Y" : "N");

            // (receive) loop run
            auto before = millis();
            can_send = mt_loop(millis());
            LOG_DEBUG("mt_look took %d ms\n", millis() - before);

            // Send loop run
            if (can_send)
            {
                SendQueueElement *msg;
                while (pdTRUE == xQueueReceive(send_q, &msg, 0))
                {
                    assert(msg != NULL);
                    bool rv = mt_send_text(msg->msg->c_str(), msg->to, msg->ch);
                    LOG_INFO("MMM >>> Sent. result: %s\n", rv ? "true" : "false");
                    delete (msg);
                    DEVICE_CONTACT();
                }
            }
            else
            {
                LOG_ERROR("MT connection not ready to send!\n");
            }

            // regulartly refresh node report.
            if ((millis() - last_nr_time) > 60000 && connected)
            {
                LOG_DEBUG("Node report %d ago\n", millis() - last_nr_time);
                connected = false; // suspend until node report fully received.
                bool rv = mt_request_node_report(meshcamper_mt_connected_cb);
                LOG_INFO("refreshed node report result: %d\n", rv);
                if (!rv)
                {
                    LOG_WARN("Node report request unsuccessful, reinitializing Meshtastic lib.");
                    init();
                }
            }
            if (last_device_contact_time() > 120000)
            {
                LOG_WARN("No device contact in 2min, reinitializing.\n");
                init();
            }

            if (connected)
            {
                delay(500);
            }
        }
    }

    void MeshtasticClient::register_handler(MessageHandler *handler)
    {
        message_handler = handler;
    }

    void MeshtasticClient::node_report(mt_node_t *node, mt_nr_progress_t progress)
    {
        if (progress == MT_NR_IN_PROGRESS)
        {
            connected = false; // skip delay in loop to consume data quickly.
            LOG_INFO("Got node report for 0x%08x\n", node->node_num);
            if (node->is_mine)
            {
                LOG_INFO("Found mine: 0x%08x\n", node->node_num);
                own_address = node->node_num;
            }
            else if (node->is_favorite)
            {
                LOG_INFO("Found trusted node: 0x%08x\n", node->node_num);
                trusted_nodes.insert(node->node_num);
            }
            else
            {
                LOG_DEBUG("Other node: 0x%08x\n", node->node_num);
            }
        }
        else if (progress == MT_NR_DONE)
        {
            LOG_INFO("Node report is done, assuming meshtastic device is up!\n");
            connected = true;
            last_nr_time = millis();
            for (auto &&favorite : trusted_nodes)
            {
                LOG_INFO("Trusted favorite: 0x%08x\n", favorite);
            }
        }
        else if (progress == MT_NR_INVALID)
        {
            LOG_ERROR("ERROR: Invalid Node Report received.\n");
        }
    }

    void MeshtasticClient::handle_message(uint32_t from, uint32_t to, uint8_t channel, const char *text)
    {
        if (channel != 0)
            return; // ignore messages for non-primary channel.

        if (!trusted_nodes.contains(from))
        {
            LOG_INFO("Ignoring message from untrusted node 0x%08x\n", from);
            return;
        }

        if (to != own_address && to != BROADCAST_ADDR)
        {
            LOG_DEBUG("Ignoring message for someone else.");
            return;
        }

        assert(message_handler);
        message_handler->handle_message(from, to, channel, std::string(text));
    }

    void MeshtasticClient::send(const std::string &message, uint32_t to)
    {
        LOG_INFO("MMM >>> Enqueuing message\"%s\" to 0x%08x\n", message.c_str(), to);
        SendQueueElement *msg = new SendQueueElement(to, 0, message);
        xQueueSend(send_q, &msg, portMAX_DELAY);
    }

    MeshtasticClient::~MeshtasticClient()
    {
    }

    MeshtasticClient::SendQueueElement::SendQueueElement(const uint32_t &to, const uint8_t &ch, const std::string &msg)
        : to(to), ch(ch), msg(new std::string(msg))
    {
    }

    uint64_t MeshtasticClient::last_device_contact_time()
    {
        return (millis() - last_device_contact);
    }

    MeshtasticClient::SendQueueElement::~SendQueueElement()
    {
        delete (msg);
    }
}
