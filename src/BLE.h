#pragma once

#include <list>

#include "NimBLEDevice.h"
#include "NimBLEScan.h"
#include "NimBLEAddress.h"

#include "Arduino.h"

#include "Logging.h"

#include "BatteryStatus.h"
#include "victron/devices/VictronBatteryMonitor.h"

namespace meshcamper
{
  extern "C"
  {
    void meshcamper_ble_task(void *ptr);
  }

  /* Bluetooth LE Client/Scanner, captures
     Victron BMV-712 instant readout data.
     */
  class MeshcamperBLEClient : public NimBLEScanCallbacks
  {
    /* freertos tas glue function */
    friend void meshcamper_ble_task(void *ptr);

    virtual void onResult(const NimBLEAdvertisedDevice *advertisedDevice);
    virtual void onDiscovered(const NimBLEAdvertisedDevice *advertisedDevice);
    virtual void onScanEnd(const NimBLEScanResults &scanResults, int reason);

  private:
    std::list<VictronDevice*> devices;

    BatteryStatus* battery_status;
    
    NimBLEScan *scanner;

    void task();

    void handle_advertisement(const NimBLEAdvertisedDevice *advertisedDevice, const std::string &type = "UNKNOWN");

    void handle_victron_instant_readout(VictronDevice* device, const std::string &raw_data);

  public:
    MeshcamperBLEClient(BatteryStatus* b);

    /* start FreeRTOS BLE Scanner thread. should never return.*/
    void start();

    void addVictronDevice(VictronDevice* device);

    virtual ~MeshcamperBLEClient();
  };

}