#include "Mutex.hpp"
#include "Task.hpp"
#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include <string>

using namespace fos;

constexpr int WAIT = 2000;
constexpr int StackSize = 256;

int CommonRessource{};

void Mutator1(Mutex &mtx) {
    while (true) {
        mtx.claim();
        Serial.printf("Mutator1: %d -> %d \n\r", CommonRessource, ++CommonRessource);
        Task::delayMs(WAIT / 2);
        mtx.release();
        Task::delayMs(WAIT / 2);
    }
}

void Mutator2(Mutex &mtx) {
    Task::delayMs(WAIT / 2);
    while (true) {
        { // Better approach than Mutator1, Mutex guard releases on exit of scope automatically
            MutexGuard guard(mtx);
            Serial.printf("Mutator2: %d -> %d \n\r", CommonRessource, ++CommonRessource);
        }
        Task::delayMs(WAIT);
    }
}

void setupTasks() {
    // WARNING needs to be static, because somehow this variable looses scope after the
    // vTaskStartScheduler is called
    static Mutex mtx;
    static TaskT<Mutex &> mutator1Task(Mutator1, mtx, "Mutator1", StackSize);
    static TaskT<Mutex &> mutator2Task(Mutator2, mtx, "Mutator2", StackSize);
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