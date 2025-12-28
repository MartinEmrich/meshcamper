#include <Arduino.h>

#include "BLE.h"
#include "Logging.h"

espjoker::BLEClient *ble;

void setup()
{
  Serial.begin(115200);
  ble = new espjoker::BLEClient();
}

void loop()
{
  Serial.println("Yeah from loop!");
  delay(1000);
}
