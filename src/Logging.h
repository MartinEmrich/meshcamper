#pragma once
#include <Arduino.h>
/* Placeholder/Boilerplate for logging. */

#define LOG(...) Serial.printf("[%07d] ", millis());Serial.printf(__VA_ARGS__)

#define LOG_ERROR LOG
#define LOG_WARN LOG
#define LOG_INFO LOG
#define LOG_DEBUG LOG
