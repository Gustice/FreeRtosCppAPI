#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unity.h>
#include "esp_log.h"

#include "task.hpp"

using namespace fos;

void setUp(void) {}

void tearDown(void) {}

void taskToSuccess() {
    TEST_ASSERT_TRUE(true);
}

void testDelay() {
    TickType_t start = xTaskGetTickCount ();
    Task::delay(5 * portTICK_PERIOD_MS);
    TickType_t end = xTaskGetTickCount ();
    TEST_ASSERT_GREATER_OR_EQUAL(5, (end-start));
    TEST_ASSERT_LESS_OR_EQUAL(6, (end-start));
}

static bool boolTask1WasCalled = false;
void task1(void *) {
    boolTask1WasCalled = true;
    while (true) {
        vTaskDelay(100);
    }
}

void checkIfTaskIsCreatedAndCalled(void) {
    static Task task(task1, "task1");
    Task::delay(50);
    TEST_ASSERT_TRUE(boolTask1WasCalled);
}

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

void app_main(void) {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(taskToSuccess);
    RUN_TEST(testDelay);
    RUN_TEST(checkIfTaskIsCreatedAndCalled);
    UNITY_END(); // stop unit testing
}
