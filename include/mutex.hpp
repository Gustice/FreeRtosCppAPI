/**
 * @file mutex.hpp
 * @author Gustice
 * @brief Mutex Wrapper
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace fos {

/**
 * @brief Mutex
 */
class Mutex {
  private:
    QueueHandle_t _handle;

  public:
    /// @brief Tick type (redefinition for convenience)
    using Tick = TickType_t;
    /// @brief Definition for endless waittime
    static constexpr Tick MaxDelay = portMAX_DELAY;

    /// @brief Constructor
    Mutex() {
        _handle = xSemaphoreCreateMutex();
        configASSERT(_handle != 0 && "Semaphore create must finish successfully");
    }

    ~Mutex() {
        vSemaphoreDelete(_handle);
    };

    /// @brief Relase mutex
    void release() {
        auto ret = xSemaphoreGive(_handle);
        configASSERT(ret == pdPASS && "Underlying semaphore queue must be initialized correcty");
    }

    /// @brief Take mutex
    /// @param timeout Timeout to wait for signal (default = wait forever)
    /// @return true if successfully claimed (not timeout)
    bool claim(Tick timeout = MaxDelay) {
        auto ret = xSemaphoreTake(_handle, timeout);
        return ret == pdPASS;
    }
};

/**
 * @brief Mutex Guard. Self releasing mutex for atomic operation
 */
class MutexGuard {
  private:
    Mutex &_mutex;
    bool _claimed;

  public:
    /// @brief Constructor
    /// @param mtx Mutex to claim
    /// @param timeout Timeout to wait for signal (default = wait forever)
    MutexGuard(Mutex &mtx, Mutex::Tick timeout = Mutex::MaxDelay) : _mutex(mtx) {
        _claimed = _mutex.claim(timeout);
    };

    ~MutexGuard() {
        if (!_claimed) {
            return;
        }

        _mutex.release();
    }

    /// @brief Check if mutex was claimed successfully
    /// @return true if claimed
    bool isActive() {
        return _claimed;
    }
};

} // namespace fos
