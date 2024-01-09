#include "Task.hpp"
#include <Arduino.h>
#include <STM32FreeRTOS.h>

using namespace fos;

constexpr int WAIT = 1000;
constexpr int StackSize = 256;
constexpr int LED = PC13;

void ParameterizedTask(int id) {
    Serial.printf("starting task #%d\n\r", id);
    uint32_t cnt{};
    while (true) {
        Serial.printf("Task #%d tick %d\n\r", id, cnt++);
        Task::delayMs(WAIT);
    }
}

void ParameterlessTask() {
    Serial.printf("starting task\n\r");
    uint32_t cnt{};
    pinMode(LED, OUTPUT);
    while (true) {
        digitalToggle(LED); // execute blink event
        // parameterless, can use globals though
        Serial.printf("tick %d\n\r", cnt++);
        Task::delayMs(WAIT);
    }
}

void setupTasks() {
    // WARNING needs to be static, because somehow this variable looses scope after the
    // vTaskStartScheduler is called
    static Task tickerTask(ParameterlessTask, "Tsk", StackSize);
    static const int taskId1 = 1;
    static const int taskId2 = 2;
    static TaskT<int> tickerTask1(ParameterizedTask, taskId1, "Tick#1", StackSize);
    static TaskT<int> tickerTask2(ParameterizedTask, taskId2, "Tick#2", StackSize);
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        // wait for serial port to connect. Needed for native USB
    }

    setupTasks();

    // FreeRTOS DOMAIN ...
    // start scheduler
    vTaskStartScheduler();
    Serial.println("Insufficient RAM");
    while (1) // error if it ends up here
        ;
}

void loop() {
    // Empty. Things are done in Tasks.
}