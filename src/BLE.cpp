#include "BLE.h"
namespace meshcamper
{

  extern "C"
  {
    void meshcamper_ble_task(void *ptr)
    {
      static_cast<meshcamper::MeshcamperBLEClient *>(ptr)->task();
    }
  }

  TaskHandle_t ble_task_handle;

  MeshcamperBLEClient::MeshcamperBLEClient(BatteryStatus *b) : battery_status(b)
  {
    BLEDevice::init("meshcamper");
  }

  void MeshcamperBLEClient::start()
  {
    scanner = BLEDevice::getScan();

    scanner->setScanCallbacks(this);

    // seems to be sufficient for Victron BLE devices
    scanner->setActiveScan(false);

    scanner->setInterval(500);
    scanner->setWindow(490);

    LOG_INFO("Looking for %d devices...", devices.size());
    xTaskCreate(meshcamper_ble_task, "BLE Task", 2048, this, 1, &ble_task_handle);
    LOG_INFO("Task created.\n");
  }

  void MeshcamperBLEClient::addVictronDevice(VictronDevice *device)
  {
    devices.push_back(device);
  }

  void MeshcamperBLEClient::task()
  {
    LOG_INFO("BLE Task started\n");
    while (true)
    {
      bool rv = scanner->start(1000, false);
      //LOG_DEBUG("Task mem watermark: %d bytes. Scanning again in 5s\n", uxTaskGetStackHighWaterMark(ble_task_handle));
      delay(5000);
    }
    vTaskDelete(NULL);
  }

  void MeshcamperBLEClient::handle_advertisement(const NimBLEAdvertisedDevice *advertisedDevice, const std::string &type)
  {
    VictronDevice *device = NULL;
    for (auto &&d : devices)
    {
      if (d->matches(advertisedDevice->getAddress()))
        device = d;
    }
    if (device == NULL)
    {
      return;
    }

    std::string manufacturer_data = advertisedDevice->getManufacturerData();
    if (manufacturer_data.size() < 2 || manufacturer_data[0] != 0xe1 || manufacturer_data[1] != 0x02)
    {
      // LOG_DEBUG("Skipping non-Victron manufaturer data\n");
      return;
    }

    if (manufacturer_data[2] != 0x10)
    { 
      // LOG_DEBUG("Skipping non-instant-readout data\n");
      return;
    }
    LOG_DEBUG("Handle_type. %s\n", type.c_str());
    handle_victron_instant_readout(device, manufacturer_data.substr(2, -1));
  }

  void MeshcamperBLEClient::handle_victron_instant_readout(VictronDevice *device, const std::string &raw_data)
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

  void MeshcamperBLEClient::onResult(const NimBLEAdvertisedDevice *advertisedDevice)
  {
    //handle_advertisement(advertisedDevice, "onResult");
  }

  void MeshcamperBLEClient::onDiscovered(const NimBLEAdvertisedDevice *advertisedDevice)
  {
    //handle_advertisement(advertisedDevice, "onDiscovered");
  }

  void MeshcamperBLEClient::onScanEnd(const NimBLEScanResults &scanResults, int reason)
  {
    LOG_INFO("onScanEnd: %d BLE Scan Results\n", scanResults.getCount());
    for (auto &&result : scanResults)
    {
      handle_advertisement(result, "onScanEnd");
    }
  }

  MeshcamperBLEClient::~MeshcamperBLEClient()
  {
  }
}