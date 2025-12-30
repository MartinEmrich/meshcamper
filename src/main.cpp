#include <Arduino.h>

#include "BLE.h"
#include "MeshtasticClient.h"
#include "MeshcamperApplication.h"
#include "Logging.h"
#include "victron/devices/VictronBatteryMonitor.h"

#include "config.h"

meshcamper::MeshcamperBLEClient *ble;

meshcamper::MeshtasticClient *mt;

meshcamper::BatteryStatus *battery_status;

meshcamper::MeshcamperApplication *meshcamper_application;

uint8_t led = 0;

uint64_t last_info = 0;

void setup()
{
  Serial.begin(115200);
  battery_status = new meshcamper::BatteryStatus();
  ble = new meshcamper::MeshcamperBLEClient(battery_status);
  mt = new meshcamper::MeshtasticClient();
  meshcamper_application = new meshcamper::MeshcamperApplication(battery_status, mt);

  ble->addVictronDevice(new VictronBatteryMonitor(VICTRON_BMV_BLE_MAC, VICTRON_BMV_INSTANT_READOUT_KEY));

  // from meshtastic example...doubt this is effective...
  randomSeed(micros());
  last_info = micros();

  ble->start();
  mt->start();
  pinMode(2, OUTPUT);
}

/* Most is done in FreeRTOS tasks, only main notifications, a heartbeat LED and serial info here */
void loop()
{
  meshcamper_application->loop();

  digitalWrite(2, 1);
  auto now = millis();
  if ((now - last_info) > 30000 && battery_status->is_ready())
  {
    Serial.println(battery_status->get_as_short_string().c_str());
    last_info = now;
  }
  delay(1000);
  digitalWrite(2, 0);
  delay(1000);
}
