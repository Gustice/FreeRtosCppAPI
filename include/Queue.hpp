/**
 * @file queue.hpp
 * @author Gustice
 * @brief Queue Wrapper
 * @date 2023-11-01
 *
 * @copyright Copyright (c) 2023
 */

#pragma once

#ifdef ARDUINO_ARCH_STM32
#include <FreeRTOS/Source/include/FreeRTOS.h>
#include <FreeRTOS/Source/include/queue.h>
#elif ESP_PLATFORM
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#endif
#include <memory>
#include <string_view>

namespace fos {

/**
 * @brief Message Queue
 *
 * @tparam T Type for message payload
 */
template <typename T> class Queue {
  private:
    bool _isActive;
    QueueHandle_t xQueue;

  public:
    /// @brief Tick type (redefinition for convenience)
    using Tick = TickType_t;
    /// @brief Definition for endless waittime
    static constexpr Tick MaxDelay = portMAX_DELAY;

    /// @brief Constructor
    /// @param size number of slots in queue
    Queue(size_t size) {
        xQueue = xQueueCreate(size, sizeof(T *));
        configASSERT(xQueue != 0 && "Queue create must finish successfully");
        _isActive = true;
    }

    ~Queue() {
        vQueueDelete(xQueue);
    }

    /// @brief Push new message in queue
    /// @param message payload
    /// @return nullptr if successful else give back pointer to queued element if pushing failed
    std::unique_ptr<T> enqueue(std::unique_ptr<T> message) {
        if (uxQueueSpacesAvailable(xQueue) == 0) {
            return message;
        }
        T *o = message.release();
        auto ret = xQueueSend(xQueue, &o, (TickType_t)0);
        configASSERT(ret == pdPASS && "Underlying semaphore queue must be initialized correcty");

        return message;
    }

    /// @brief Pop message from queue
    /// @param timeout timeout to wait for message (default: wait for ever)
    /// @return pointer to dequeued element
    std::unique_ptr<T> dequeue(Tick timeout = MaxDelay) {
        T *pMsg = nullptr;
        if (!xQueueReceive(xQueue, &pMsg, timeout)) {
            return {nullptr};
        }
        return std::unique_ptr<T>(pMsg);
    }

    /// @brief Pop latest element from queue and ignore all older elements
    /// @param timeout timeout to wait for message (default: wait for ever)
    /// @return pointer to dequeued element
    std::unique_ptr<T> dequeueLatest(Tick timeout = MaxDelay) {
        T *pMsg = nullptr;
        if (!xQueueReceive(xQueue, &pMsg, timeout)) {
            return {nullptr};
        }

        while (true == xQueueReceive(xQueue, &pMsg, (TickType_t)0)) {
        }
        return std::unique_ptr<T>(pMsg);
    }

    /// @brief terminate message queue
    void signalClose() {
        _isActive = false;
    }

    /// @brief Check if message queue is active
    /// @return true if active
    bool isActive() {
        return _isActive;
    }
};

} // namespace fos