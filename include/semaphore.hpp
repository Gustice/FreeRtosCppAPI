#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace fos {

class Semaphore {
  private:
    enum Type { Binary, Counting };
    const Type SemType;
    QueueHandle_t _handle;

  public:
    using Tick = TickType_t;

    Semaphore() : SemType(Semaphore::Binary) {
        _handle = xSemaphoreCreateBinary();
        configASSERT(_handle != 0 && "Semaphore create must finish successfully");
    }

    Semaphore(uint maxCount, uint init = 0) : SemType(Semaphore::Counting) {
        _handle = xSemaphoreCreateCounting(maxCount, init);
        configASSERT(_handle != 0 && "Semaphore create must finish successfully");
    }

    ~Semaphore() {
        vSemaphoreDelete(_handle);
    };

    void give() {
        auto ret = xSemaphoreGive(_handle);
        configASSERT(ret == pdPASS && "Underlying semaphore queue must be initialized correcty");
    }

    bool take(Tick timeout = portMAX_DELAY) {
        auto ret = xSemaphoreTake(_handle, timeout);
        return ret == pdPASS;
    }
};

} // namespace fos
