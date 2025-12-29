#include "BLE.h"
namespace espjoker
{

  extern "C"
  {
    void espjoker_ble_task(void *ptr)
    {
      static_cast<espjoker::ESPJokerBLEClient *>(ptr)->task();
    }
  }

  TaskHandle_t task_handle;

  ESPJokerBLEClient::ESPJokerBLEClient(BatteryStatus *b) : battery_status(b)
  {
    BLEDevice::init("esp-joker");
  }

  void ESPJokerBLEClient::start()
  {
    scanner = BLEDevice::getScan();

    scanner->setScanCallbacks(this);
    scanner->setActiveScan(true);
    scanner->setInterval(500);
    scanner->setWindow(490);

    LOG_INFO("Looking for %d devices...", devices.size());
    xTaskCreate(espjoker_ble_task, "BLE Task", 2048, this, 1, &task_handle);
    LOG_INFO("Task created.\n");
  }

  void ESPJokerBLEClient::addVictronDevice(VictronDevice *device)
  {
    devices.push_back(device);
  }

  void ESPJokerBLEClient::task()
  {
    LOG_INFO("BLE Task started");
    while (true)
    {
      LOG_INFO("Task loop start.\n");
      bool rv = scanner->start(1000, false);
      LOG_INFO("BLE Scan complete, result was %s\n", rv ? "true" : "false");
      LOG_DEBUG("Task mem watermark: %d bytes. Scanning again in 5s\n", uxTaskGetStackHighWaterMark(task_handle));
      delay(5000);
    }
    vTaskDelete(NULL);
  }

  void ESPJokerBLEClient::handle_advertisement(const NimBLEAdvertisedDevice *advertisedDevice, const std::string &type)
  {
    VictronDevice *device = NULL;
    for (auto &&d : devices)
    {
      if (d->matches(advertisedDevice->getAddress()))
        device = d;
    }
    if (device == NULL)
    {
      // LOG_DEBUG("Ignoring unknown device %s\n", advertisedDevice->getAddress().toString().c_str());
      return;
    }

    LOG_INFO("Got Victron Device by MAC: %s\n", advertisedDevice->getAddress().toString().c_str());

    std::string manufacturer_data = advertisedDevice->getManufacturerData();
    if (manufacturer_data.size() < 2 || manufacturer_data[0] != 0xe1 || manufacturer_data[1] != 0x02)
    {
      LOG_DEBUG("Skipping non-Victron manufaturer data\n");
      return;
    }

    if (manufacturer_data[2] != 0x10)
    {
      LOG_DEBUG("Skipping non-instant-readout data\n");
      return;
    }
    handle_victron_instant_readout(device, manufacturer_data.substr(2, -1));
  }

  void ESPJokerBLEClient::handle_victron_instant_readout(VictronDevice *device, const std::string &raw_data)
  {
    try
    {
      switch (device->get_device_class())
      {

      case VICTRON_DEVICE_CLASS_BATTERYMONITOR:
      {
        VictronBatteryMonitorData *data = static_cast<VictronBatteryMonitor *>(device)->parse_data(raw_data);
        battery_status->update(data);
#ifdef DEBUG_DECODE        
        LOG_DEBUG("Model: 0x%04x, Readout type 0x%02x\n", data->get_model_id(), data->get_readout_type());

        std::string d = data->get_decrypted();
        LOG_DEBUG("Decrypted: ");
        for (auto &&c : d)
        {
          LOG_DEBUG("%02x ", c);
        }

        LOG_DEBUG("\n***** Battery: %0.2fV, Starter: %0.2fV, current: %0.3fA, SOC: %.1f%%\n\n",
                  data->main_voltage,
                  data->aux_voltage,
                  data->current,
                  data->soc);

          
        LOG_DEBUG("\n");
#endif
        delete (data);
      }
      break;
      default:
      {
        LOG_INFO("Ignoring non-BMV-712 device.");
        return;
      }
      break;
      }
    }
    catch (VictronError e)
    {
      LOG_ERROR("Victron data error: %s", e.get_cause().c_str());
    }
  }

  /* BLE Scan callbacks */

  void ESPJokerBLEClient::onResult(const NimBLEAdvertisedDevice *advertisedDevice)
  {
    handle_advertisement(advertisedDevice, "onResult");
  }

  void ESPJokerBLEClient::onDiscovered(const NimBLEAdvertisedDevice *advertisedDevice)
  {
    handle_advertisement(advertisedDevice, "onDiscovered");
  }

  void ESPJokerBLEClient::onScanEnd(const NimBLEScanResults &scanResults, int reason)
  {
    LOG_INFO("onScanEnd: %d BLE Scan Results are in!\n", scanResults.getCount());
    for (auto &&result : scanResults)
    {
      handle_advertisement(result, "onScanEnd");
    }
    LOG_INFO("BLE Scan Results: Thats it.\n");
  }

  ESPJokerBLEClient::~ESPJokerBLEClient()
  {
  }
}