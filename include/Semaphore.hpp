/**
 * @file semaphore.hpp
 * @author Gustice
 * @brief Semaphore Wrapper
 * @date 2023-11-01
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

#ifdef ARDUINO_ARCH_STM32
#include <FreeRTOS/Source/include/FreeRTOS.h>
#include <FreeRTOS/Source/include/semphr.h>
#elif ESP_PLATFORM
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#endif

namespace fos {

/**
 * @brief Semaphore
 */
class Semaphore {
  private:
    enum Type {
        Binary,  ///< Binary semaphore, can only be true or false, no matter how often given
        Counting ///< Counting semaphore, keeps track of number of give and takes
    };

    const Type SemType;
    QueueHandle_t _handle;

  public:
    /// @brief Tick type (redefinition for convenience)
    using Tick = TickType_t;

    /// @brief Definition for endless waittime
    static constexpr Tick MaxDelay = portMAX_DELAY;

    /// @brief Constructor for binary semaphore
    Semaphore() : SemType(Semaphore::Binary) {
        _handle = xSemaphoreCreateBinary();
        configASSERT(_handle != 0 && "Semaphore create must finish successfully");
    }

    /// @brief Constructor for conting semaphore
    /// @param maxCount Counting limit
    /// @param init Initial count
    Semaphore(uint32_t maxCount, uint32_t init = 0) : SemType(Semaphore::Counting) {
        _handle = xSemaphoreCreateCounting(maxCount, init);
        configASSERT(_handle != 0 && "Semaphore create must finish successfully");
    }

    ~Semaphore() {
        vSemaphoreDelete(_handle);
    };

    /// @brief Give signal
    /// @note Binary semaphore can only be effectively given once
    void give() {
        // checking for error would lead to false positives in case of multiple gives ...
        xSemaphoreGive(_handle);
    }

    /// @brief Consume signal
    /// @param timeout Timeout to wait for signal (default = wait forever)
    /// @return true if successfully consumed (not timeout)
    bool take(Tick timeout = MaxDelay) {
        auto ret = xSemaphoreTake(_handle, timeout);
        return ret == pdPASS;
    }
};

} // namespace fos
