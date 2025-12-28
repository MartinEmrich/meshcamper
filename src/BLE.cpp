#include "BLE.h"
namespace espjoker
{

  extern "C"
  {
    void espjoker_ble_task(void *ptr)
    {
      static_cast<espjoker::BLEClient *>(ptr)->task();
    }
  }

  TaskHandle_t task_handle;

  BLEClient::BLEClient()
  {
    BLEDevice::init("espjoker");

    LOG_INFO("Creating BLE Task");
    xTaskCreate(espjoker_ble_task, "BLE Task", 2048, NULL, 1, &task_handle);
    LOG_INFO("Task created.");
  }

  void BLEClient::task()
  {
    LOG_INFO("BLE Task started");
    while (true)
    {
      LOG_DEBUG("Test from BLE task");
      LOG_DEBUG("Task mem watermark: %d", uxTaskGetStackHighWaterMark(task_handle));
      delay(1200);
    }
    vTaskDelete(NULL);
  }

  BLEClient::~BLEClient()
  {
  }
}