#pragma once

#include "NimBLEDevice.h"
#include "Arduino.h"
#include "Logging.h"


namespace espjoker
{
extern "C"
  {
    void espjoker_ble_task(void *ptr);
  }
  
  class BLEClient
  {
    friend void espjoker_ble_task(void *ptr);

  private:
    void task();

  public:
    BLEClient();

    virtual ~BLEClient();
  };

}