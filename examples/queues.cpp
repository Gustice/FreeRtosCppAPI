#include "Queue.hpp"
#include "Task.hpp"
#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include <string>

using namespace fos;

constexpr int WAIT = 1000;
constexpr int StackSize = 256;

struct TxMsg {
    // Currently a message object needs an constructor in order to use make_unique
    // This is obsolete in c++20
    // make_unique does not leek memory if initialisation of unique pointer fails
    TxMsg(uint32_t c, std::string m) : cnt(c), message(m){};
    uint32_t cnt;
    std::string message;
};

void Provider(Queue<TxMsg> &queue) {
    uint32_t cnt{};
    while (true) {
        auto e = std::make_unique<TxMsg>(cnt++, "tick");
        queue.enqueue(std::move(e));
        Task::delayMs(WAIT);
    }
}

void Consumer(Queue<TxMsg> &queue) {
    while (true) {
        auto msg = queue.dequeue(); // Wait with infinit timeout, guaranteed to succeed
        Serial.printf("Message: %s %d\n\r", msg->message.c_str(), msg->cnt);
    }
}

void setupTasks() {
    // WARNING needs to be static, because somehow this variable looses scope after the
    // vTaskStartScheduler is called
    static Queue<TxMsg> queue(8);
    static TaskT<Queue<TxMsg> &> providerTask(Provider, queue, "Provider", StackSize);
    static TaskT<Queue<TxMsg> &> consumerTask(Consumer, queue, "Consumer", StackSize);
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