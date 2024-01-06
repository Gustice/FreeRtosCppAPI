#ifdef ARDUINO_ARCH_STM32

#include <Arduino.h>
#include <STM32FreeRTOS.h>

constexpr int LED = PC13;
constexpr int WAIT = 500;

// define two tasks for Blink & AnalogRead
void TaskBlink(void *pvParameters);

// the setup function runs once when you press reset or power the board
void setup() {

    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);

    // while (!Serial) {
    //     ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and
    //       // other 32u4 based boards.
    // }

    xTaskCreate(TaskBlink, (const portCHAR *)"Blink", 128, NULL, 2, NULL);

    // start scheduler
    vTaskStartScheduler();
    Serial.println("Insufficient RAM");
    while (1) // error if it ends up here
        ;
}

void loop() {
    // Empty. Things are done in Tasks.
}

void TaskBlink(void *pvParameters) {
    pinMode(LED, OUTPUT);
    while (true) {
        digitalWrite(LED, HIGH);
        osDelay(WAIT);
        digitalWrite(LED, LOW);
        osDelay(WAIT);
    }
}

#elif ESP_PLATFORM

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>

#include "mutex.hpp"
#include "queue.hpp"
#include "semaphore.hpp"
#include "task.hpp"

using namespace fos;

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
