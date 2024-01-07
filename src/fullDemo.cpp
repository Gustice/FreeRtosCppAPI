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

#include "mutex.hpp"
#include "queue.hpp"
#include "semaphore.hpp"
#include "task.hpp"

using namespace fos;
using namespace std::literals::string_view_literals;

constexpr int WAIT = 500;

// define two tasks for Blink & AnalogRead
void TaskBlink(int waitMs) {
    Serial.println("starting blink task");
    pinMode(LED, OUTPUT);
    while (true) {
        digitalToggle(LED);
        Task::delayMs(waitMs);
    }
}

void TaskTick() {
    uint32_t cnt{};
    while (true) {
        Serial.printf("tick %d\n\r", cnt++);
        Task::delayMs(1000);
    }
}

void setupTasks() {
    static TaskT<int> blinkerTask(TaskBlink, WAIT, "Blink"sv, StackSize);
    static Task tickerTask(TaskTick, "Tick"sv, StackSize);
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