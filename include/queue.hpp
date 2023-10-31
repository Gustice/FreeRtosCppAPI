#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <memory>
#include <string_view>

namespace fos {

template <typename T> class MessageQueue {
  private:
    bool _isActive;
    QueueHandle_t xQueue;

  public:
    using Tick = TickType_t;
    static constexpr Tick MaxDelay = portMAX_DELAY;

    MessageQueue(size_t size) {
        xQueue = xQueueCreate(size, sizeof(T *));
        configASSERT(xQueue != 0 && "Queue create must finish successfully");
        _isActive = true;
    }

    ~MessageQueue() {
        vQueueDelete(xQueue);
    }

    std::unique_ptr<T> enqueue(std::unique_ptr<T> message) {
        if (uxQueueSpacesAvailable(xQueue) == 0) {
            return message;
        }
        T *o = message.release();
        auto ret = xQueueSend(xQueue, &o, (TickType_t)0);
        configASSERT(ret == pdPASS && "Underlying semaphore queue must be initialized correcty");

        return message;
    }

    std::unique_ptr<T> dequeue(Tick timeout = MaxDelay) {
        T *pMsg = nullptr;
        if (!xQueueReceive(xQueue, &pMsg, timeout)) {
            return {nullptr};
        }
        return std::unique_ptr<T>(pMsg);
    }

    std::unique_ptr<T> dequeueLatest(Tick timeout = MaxDelay) {
        T *pMsg = nullptr;
        if (!xQueueReceive(xQueue, &pMsg, timeout)) {
            return {nullptr};
        }

        while (true == xQueueReceive(xQueue, &pMsg, (TickType_t)0)) {
        }
        return std::unique_ptr<T>(pMsg);
    }

    void signalClose() {
        _isActive = false;
    }

    bool isActive() {
        return _isActive;
    }
};

} // namespace fos