#include <Arduino.h>

#include "BLE.h"
#include "Logging.h"
#include "victron/devices/VictronBatteryMonitor.h"

#include "config.h"

espjoker::ESPJokerBLEClient *ble;

void setup()
{
  Serial.begin(115200);
  ble = new espjoker::ESPJokerBLEClient();

  ble->addVictronDevice(new VictronBatteryMonitor(VICTRON_BMV_BLE_MAC, VICTRON_BMV_INSTANT_READOUT_KEY));

  ble->start();
}

void loop()
{
  Serial.println("still alive!\n");
  delay(30000);
}
