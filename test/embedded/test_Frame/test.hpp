#ifdef ARDUINO_ARCH_STM32

#include <Arduino.h>
#include <STM32FreeRTOS.h>
constexpr int StackSize = 256;

#elif ESP_PLATFORM

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>
constexpr int StackSize = 1024;

#endif

#include <unity.h>

class TimeTest {
  private:
    bool running{};
    TickType_t _begin;

  public:
    TimeTest() {
        start();
    }

    void start() {
        _begin = xTaskGetTickCount();
        running = true;
    }
    

    TickType_t getRunTime() {
        return xTaskGetTickCount() - _begin;
    }
};