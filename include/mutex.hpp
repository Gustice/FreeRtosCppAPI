#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace fos {

class Mutex {
  private:
    QueueHandle_t _handle;

  public:
    using Tick = TickType_t;
    static constexpr Tick MaxDelay = portMAX_DELAY;

    Mutex() {
        _handle = xSemaphoreCreateMutex();
        configASSERT(_handle != 0 && "Semaphore create must finish successfully");
    }

    ~Mutex() {
        vSemaphoreDelete(_handle);
    };

    void release() {
        auto ret = xSemaphoreGive(_handle);
        configASSERT(ret == pdPASS && "Underlying semaphore queue must be initialized correcty");
    }

    bool claim(Tick timeout = MaxDelay) {
        auto ret = xSemaphoreTake(_handle, timeout);
        return ret == pdPASS;
    }
};

class MutexGuard {
  private:
    Mutex &_mutex;
    bool _claimed;

  public:
    MutexGuard(Mutex &mtx, Mutex::Tick timeout = Mutex::MaxDelay) : _mutex(mtx) {
        _claimed = _mutex.claim();
    };

    ~MutexGuard() {
        if (!_claimed) {
            return;
        }

        _mutex.release();
    }
};

} // namespace fos
