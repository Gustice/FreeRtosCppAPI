#if !defined(TASK_H)
#define TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cassert>

namespace fos {

class Task {
    using Priority = unsigned short;
    static constexpr size_t DEFAULT_STACKSIZE = 0x400; // 4k
    static constexpr Priority MIN_PRIORITY = tskIDLE_PRIORITY;
    static constexpr Priority MAX_PRIORITY = configMAX_PRIORITIES;
    using TaskSignature = void (*)(void *pvParameters);

    TaskHandle_t _handle;

  private:
  public:
    Task(TaskSignature call, const char *name) {
        // @todo ?? name.length < configMAX_TASK_NAME_LEN
        auto ret = xTaskCreate(call, name, DEFAULT_STACKSIZE, nullptr, tskIDLE_PRIORITY, &_handle);
        configASSERT(pdPASS == ret && "Task create must finish successfully");
    }
    ~Task() {
        // @todo this is dangerous on multi-core systems. See IDF-doc
        // vTaskDelete(_handle); 
        // task needs to clear itself
        // NOTE: INCLUDE_vTaskDelete must be defined to 1 in order to compile
    }

    static void CreateStaticTask(TaskSignature call, const char *name) {
        // @todo create a static call
    }

    static void delay(unsigned int msToDelay) {
        vTaskDelay(msToDelay / portTICK_PERIOD_MS);
    }
};

} // namespace fos

#endif // TASK_H
