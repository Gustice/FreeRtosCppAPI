/**
 * @file task.hpp
 * @author Gustice
 * @brief Task Wrapper
 * @date 2023-11-01
 *
 * @copyright Copyright (c) 2023
 */

#pragma once

#ifdef ARDUINO_ARCH_STM32
#include <FreeRTOS/Source/include/FreeRTOS.h>
#include <FreeRTOS/Source/include/task.h>
#define DEFAULT_STACKSIZE 0x100UL

#elif ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define DEFAULT_STACKSIZE 0x400UL
#endif

#include <cassert>
#include <string_view>

namespace fos {

/**
 * @brief Base class for tasks
 */
class TaskBase {
  protected:
    TaskHandle_t _handle;

    static void disposeFrameConditionally() {
        if constexpr (INCLUDE_vTaskDelete != 0) {
            vTaskDelete(NULL);
            // NOTE: This only compiles if symbol INCLUDE_vTaskDelete is defined
        }
    }

  public:
    /// @brief Priority type (redefinition for convenience)
    using Priority = unsigned short;

    /// @brief Default Stack size
    static constexpr size_t DefaultStackSize = DEFAULT_STACKSIZE;

    /// @brief Lowest priority for new tasks
    static constexpr Priority MIN_PRIORITY = tskIDLE_PRIORITY;

    /// @brief Highest priority for new tasks
    static constexpr Priority MAX_PRIORITY = configMAX_PRIORITIES;

    /// @brief Kill a task
    /// @note It can lead to undefined behaviour when one task deletes another task on a
    ///   multi-core system. Hence a task should clear itself.
    void kill() {
        vTaskDelete(_handle);
    }

    /// @brief Delay function
    /// @param msToDelay Milliseconds to delay
    /// @note The minimal resolution for the delay is tick based
    static void delayMs(unsigned int msToDelay) {
        vTaskDelay(msToDelay / portTICK_PERIOD_MS);
    }

    /// @brief Delay function
    /// @param ticksToDelay Ticks to delay
    static void delayTicks(unsigned int ticksToDelay) {
        vTaskDelay(ticksToDelay);
    }

    virtual ~TaskBase() {
        kill();
    }
};

/**
 * @brief Simple task without task parameter
 */
class Task : public TaskBase {
  private:
    /// @brief Signature for task-implementation-callback
    using TaskSignature = void (*)();

    static void TaskFrame(void *_this) {
        reinterpret_cast<Task *>(_this)->_paramVoid.call();
        // Tasks shall not return, however if this happens dispose task
        disposeFrameConditionally();
    }

    struct Param {
        TaskSignature call;
    };
    Param _paramVoid;

  public:
    /// @brief Constructor for task without passed argument
    /// @param call Function pointer to implementation
    /// @param name Name of task
    /// @param stack Stacksize if not default
    /// @param prio Priority if not default
    Task(TaskSignature call, std::string_view name, size_t stack = DefaultStackSize,
         Priority prio = MIN_PRIORITY)
        : _paramVoid{call} {
        auto ret = xTaskCreate(TaskFrame, name.data(), stack, this, MIN_PRIORITY, &_handle);
        configASSERT(pdPASS == ret && "Task create must finish successfully");
    }
};

/**
 * @brief Task with task parameter
 * @tparam T Type of task parameter on call
 */
template <typename T> class TaskT : public TaskBase {
  private:
    /// @brief Signature for task-implementation-callback
    using TaskSignature = void (*)(T);

    template <typename TS> struct Param {
        TaskSignature call;
        const TS &arg;
    };
    Param<T> _param;

    static void TaskFrame(void *_this) {
        auto task = reinterpret_cast<TaskT *>(_this);
        task->_param.call(task->_param.arg);
        // Tasks shall not return, however if this happens dispose task
        disposeFrameConditionally();
    }

  public:
    /// @brief Constructor for task with passed argument
    /// @param call Function pointer to implementation
    /// @param param Parameter ot pass task implementation
    /// @param name Name of task
    /// @param stack Stacksize if not default
    /// @param prio Priority if not default
    TaskT(TaskSignature call, const T &param, std::string_view name,
          size_t stack = DefaultStackSize, Priority prio = MIN_PRIORITY)
        : _param{call, param} {
        auto ret = xTaskCreate(TaskFrame, name.data(), stack, this, MIN_PRIORITY, &_handle);
        configASSERT(pdPASS == ret && "Task create must finish successfully");
    }
};

} // namespace fos
