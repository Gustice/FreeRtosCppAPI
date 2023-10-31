#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cassert>

namespace fos {

class Task {
  private:
    using Priority = unsigned short;
    static constexpr size_t DEFAULT_STACKSIZE = 0x400; // 4k
    static constexpr Priority MIN_PRIORITY = tskIDLE_PRIORITY;
    static constexpr Priority MAX_PRIORITY = configMAX_PRIORITIES;
    using TaskSignature = void (*)(void *);
    TaskHandle_t _handle;

  public:
    Task(TaskSignature call, const char *name, size_t stack = DEFAULT_STACKSIZE,
         Priority prio = MIN_PRIORITY) {
        auto ret = xTaskCreate(call, name, DEFAULT_STACKSIZE, this, MIN_PRIORITY, &_handle);
        configASSERT(pdPASS == ret && "Task create must finish successfully");
    }

    ~Task() = default;
    // NOTE It can lead to undefined behaviour when one task deletes another task on a
    // multi-core system. Hence a task should clear itself.

    static void delay(unsigned int msToDelay) {
        vTaskDelay(msToDelay / portTICK_PERIOD_MS);
    }

    static void TaskFrame(void *_this) {
        vTaskDelete(NULL); // Delete this task if callback ever returns
                           // NOTE: This compiles only if INCLUDE_vTaskDelete is defined to 1
    }
};

} // namespace fos
