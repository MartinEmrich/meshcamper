#include <Arduino.h>

#include "BLE.h"
#include "Logging.h"
#include "victron/devices/VictronBatteryMonitor.h"

#include "config.h"

espjoker::ESPJokerBLEClient *ble;

espjoker::BatteryStatus* battery_status;

void setup()
{
  Serial.begin(115200);
  battery_status = new espjoker::BatteryStatus();
  ble = new espjoker::ESPJokerBLEClient(battery_status);

  ble->addVictronDevice(new VictronBatteryMonitor(VICTRON_BMV_BLE_MAC, VICTRON_BMV_INSTANT_READOUT_KEY));

  ble->start();
}

void loop()
{
  Serial.println("still alive!\n");
  Serial.println(battery_status->get_as_short_string().c_str());
  delay(10000);
}
