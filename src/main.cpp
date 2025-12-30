#include <Arduino.h>

#include "BLE.h"
#include "MeshtasticClient.h"
#include "Logging.h"
#include "victron/devices/VictronBatteryMonitor.h"

#include "config.h"

espjoker::ESPJokerBLEClient *ble;

espjoker::MeshtasticClient *mt;

espjoker::BatteryStatus *battery_status;

uint8_t led = 0;

#define send_interval_ms 120000
uint64_t last_sent = 0;

void setup()
{
  Serial.begin(115200);
  battery_status = new espjoker::BatteryStatus();
  ble = new espjoker::ESPJokerBLEClient(battery_status);
  mt = new espjoker::MeshtasticClient();

  ble->addVictronDevice(new VictronBatteryMonitor(VICTRON_BMV_BLE_MAC, VICTRON_BMV_INSTANT_READOUT_KEY));

  // from meshtastic example...doubt this is effective...
  randomSeed(micros());
  last_sent=micros();

  ble->start();
  mt->start();
  pinMode(2, OUTPUT);
}

void loop()
{
  digitalWrite(2, 1);
  Serial.println(battery_status->get_as_short_string().c_str());
  auto now = millis();
  if ((now - last_sent) > send_interval_ms && battery_status->is_ready()) {
    mt->send(battery_status->get_as_short_string().c_str());
    last_sent = now;
  }
  delay(1000);
  digitalWrite(2, 0);
  delay(10000);
}
