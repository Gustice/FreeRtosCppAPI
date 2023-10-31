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
    static constexpr Tick MaxDelay = portMAX_DELAY;

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
        xSemaphoreGive(_handle);
        // NOTE Binary semaphore can only given once.Next give leads to error
        // auto ret = xSemaphoreGive(_handle);
        // configASSERT(ret == pdPASS && "Underlying semaphore queue must be initialized correcty");
    }

    bool take(Tick timeout = MaxDelay) {
        auto ret = xSemaphoreTake(_handle, timeout);
        return ret == pdPASS;
    }
};

} // namespace fos
