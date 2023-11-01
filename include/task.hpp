/**
 * @file task.hpp
 * @author Gustice
 * @brief Task Wrapper
 * @date 2023-11-01
 *
 * @copyright Copyright (c) 2023
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cassert>

namespace fos {

/**
 * @brief Task
 */
class Task {
  private:
    /// @brief Signature for task-implementation-callback
    using TaskVoidSignature = void (*)();
    using TaskSignature = void (*)(void *);
    TaskHandle_t _handle;

    static void TaskFrameVoid(void *_this) {
        reinterpret_cast<Task *>(_this)->_paramVoid.call();
        vTaskDelete(NULL); 
        // Delete this task if callback ever returns
                           // NOTE: This compiles only if INCLUDE_vTaskDelete is defined to 1
    }

    static void TaskFrame(void *_this) {
        auto task = reinterpret_cast<Task *>(_this);
        task->_param.call(task->_param.arg);
        vTaskDelete(NULL); 
        // Delete this task if callback ever returns
        // NOTE: This compiles only if INCLUDE_vTaskDelete is defined to 1
    }


    struct ParamVoid {
      TaskVoidSignature call;
    };
    ParamVoid _paramVoid;

    struct Param {
      TaskSignature call;
      void * arg;
    };
    Param _param;


    public :
        /// @brief Priority type (redefinition for convenience)
        using Priority = unsigned short;

    /// @brief Default Stack size
    static constexpr size_t DEFAULT_STACKSIZE = 0x400; // 4k

    /// @brief Lowes priority for new tasks
    static constexpr Priority MIN_PRIORITY = tskIDLE_PRIORITY;

    /// @brief Highest priority for new tasks
    static constexpr Priority MAX_PRIORITY = configMAX_PRIORITIES;

    /// @brief Constructor for task without passed argument
    /// @param call Function pointer to implementation
    /// @param name Name of task
    /// @param stack Stacksize if not default
    /// @param prio Priority if not default
    Task(TaskVoidSignature call, const char *name, size_t stack = DEFAULT_STACKSIZE,
         Priority prio = MIN_PRIORITY) : _paramVoid{call} {
        auto ret = xTaskCreate(TaskFrameVoid, name, DEFAULT_STACKSIZE, this, MIN_PRIORITY, &_handle);
        configASSERT(pdPASS == ret && "Task create must finish successfully");
    }

    /// @brief Constructor for task with passed argument
    /// @tparam T Type of task argument
    /// @param call Function pointer to implementation
    /// @param param Parameter ot pass task implementation
    /// @param name Name of task
    /// @param stack Stacksize if not default
    /// @param prio Priority if not default
    template <typename T>
    Task(TaskSignature call, T &param, const char *name, size_t stack = DEFAULT_STACKSIZE,
         Priority prio = MIN_PRIORITY) : _param{call, &param} {
        auto ret = xTaskCreate(TaskFrame, name, DEFAULT_STACKSIZE, this, MIN_PRIORITY, &_handle);
        configASSERT(pdPASS == ret && "Task create must finish successfully");
    }

    ~Task() = default;

    /// @brief Kill a task
    /// @note It can lead to undefined behaviour when one task deletes another task on a
    ///   multi-core system. Hence a task should clear itself.
    void kill() {
        vTaskDelete(_handle);
    }

    /// @brief Delay function
    /// @param msToDelay Milliseconds to delay
    /// @note The minimal resolution for the delay is tick based
    static void delay(unsigned int msToDelay) {
        vTaskDelay(msToDelay / portTICK_PERIOD_MS);
    }
};

} // namespace fos
