#pragma once

#define LOG(...) Serial.printf(__VA_ARGS__)

#define LOG_WARN LOG
#define LOG_INFO LOG
#define LOG_DEBUG LOG
