#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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