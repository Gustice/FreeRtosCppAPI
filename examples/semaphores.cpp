#include "Semaphore.hpp"
#include "Task.hpp"
#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include <string>

using namespace fos;

constexpr int WAIT = 1000;
constexpr int StackSize = 256;

void Provider(Semaphore &sema) {
    while (true) {
        sema.give();
        Task::delayMs(WAIT);
    }
}

void Consumer(Semaphore &sema) {
    uint32_t cnt{};
    while (true) {
        sema.take(); // Wait with infinit timeout, guaranteed to succeed
        Serial.printf("Tick %d\n\r", cnt++);
    }
}

void setupTasks() {
    // WARNING needs to be static, because somehow this variable looses scope after the
    // vTaskStartScheduler is called
    static Semaphore sema;
    static TaskT<Semaphore &> providerTask(Provider, sema, "Provider", StackSize);
    static TaskT<Semaphore &> consumerTask(Consumer, sema, "Consumer", StackSize);
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