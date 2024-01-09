/**
 * @file fullDemo.cpp
 * @author Gustice
 * @brief Coherent demonstration of FreeRTOS-Abstraction in small environment
 * @copyright Copyright (c) 2024
 */
#ifdef ARDUINO_ARCH_STM32

#include <Arduino.h>
#include <STM32FreeRTOS.h>
constexpr int StackSize = 256;
constexpr int LED = PC13;

#elif ESP_PLATFORM

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>
constexpr int StackSize = 1024;
constexpr int LED = 2;

#endif

#include "Mutex.hpp"
#include "Queue.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"
#include <string>

using namespace fos;
using namespace std::literals::string_view_literals;

constexpr int WAIT = 1000;

/// @brief Arbitrary definition for message type (to be used as queue template parameter)
struct TxMsg {
    TxMsg(uint32_t c, std::string m) : cnt(c), message(m){};
    uint32_t cnt;
    std::string message;
};

/// @brief Global semaphore for blink tasks
Semaphore blinkSem;

/// @brief Parameterless task entry for generation of blink signals
/// @note uses global semaphore
void TaskBlinkProvider() {
    Serial.println("starting blink provider task");
    while (true) {
        Task::delayMs(WAIT); // wait
        blinkSem.give();     // then signal a blink event
    }
}

/// @brief Parameterless task entry for execution of blink signals
/// @note uses global semaphore
void TaskBlink() {
    Serial.println("starting blink execution task");
    pinMode(LED, OUTPUT);
    while (true) {
        blinkSem.take();      // wait for blink event signal
        digitalWrite(LED, 0); // execute blink event
        Task::delayMs(100);   // wait
        digitalWrite(LED, 1); // execute blink event
        Serial.printf("Blink tick\n\r");
    }
}

/// @brief Parametrized task entry for message provider (generates message object)
/// @param queue reference to appropriate queue
void MessageProvider(Queue<TxMsg> &queue) {
    uint32_t cnt{};
    while (true) {
        auto e = std::make_unique<TxMsg>(cnt++, "tick");
        queue.enqueue(std::move(e));
        Task::delayMs(WAIT);
    }
}

/// @brief Parametrized task entry for message consumer (sends message over UART)
/// @param queue reference to appropriate queue
void MessageConsumer(Queue<TxMsg> &queue) {
    while (true) {
        auto msg = queue.dequeue(); // Wait with infinit timeout, guaranteed to succeed
        Serial.printf("Message: %s %d\n\r", msg->message.c_str(), msg->cnt);
    }
}

void setupTasks() {
    /// @note: All definitions as static instances to not be destructed after loosing scope
    static Queue<TxMsg> txQueue(8);
    static Task blinkerTask(TaskBlink, "BlinkExec", StackSize);
    static Task blinkeProviderTask(TaskBlinkProvider, "BlinkFlags", StackSize);

    static TaskT<Queue<TxMsg> &> tickerTask(MessageProvider, txQueue, "Tick", StackSize);
    static TaskT<Queue<TxMsg> &> senderTask(MessageConsumer, txQueue, "Sender", StackSize);
}

#ifdef ARDUINO_ARCH_STM32

// the setup function runs once when you press reset or power the board
void setup() {
    // initialize serial communication at 9600 bits per second:
    Serial.begin(115200);
    while (!Serial) {
        // wait for serial port to connect. Needed for native USB
    }

    setupTasks();
    // start scheduler
    vTaskStartScheduler();

    Serial.println("Insufficient RAM");
    while (1) // error if it ends up here
        ;
}

void loop() {
    // Empty. Things are done in Tasks.
}

#elif ESP_PLATFORM

const int StackSize = 2048;

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

static bool taskWasCalled = false;
void task1() {
    ESP_LOGI("Test", "subroutinge called");
    taskWasCalled = true;
    while (true) {
        vTaskDelay(100);
    }
}

void app_main() {

    static Task task(task1, "task1");

    while (true) {
        ESP_LOGI("main", "tick");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

#endif