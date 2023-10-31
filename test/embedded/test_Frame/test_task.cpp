#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unity.h>

#include "task.hpp"

using namespace fos;

void testDelay() {
    TickType_t start = xTaskGetTickCount();
    Task::delay(5 * portTICK_PERIOD_MS);
    TickType_t end = xTaskGetTickCount();
    TEST_ASSERT_GREATER_OR_EQUAL(5, (end - start));
    TEST_ASSERT_LESS_OR_EQUAL(6, (end - start));
}

static bool taskWasCalled = false;
static void callableTask(void *) {
    taskWasCalled = true;
    while (true) {
        vTaskDelay(100);
    }
}

void checkIfTaskIsCreatedAndCalled(void) {
    static Task task(callableTask, "task1");
    Task::delay(50);
    TEST_ASSERT_TRUE(taskWasCalled);
}

void runTaskTests(void) {
    RUN_TEST(testDelay);
    RUN_TEST(checkIfTaskIsCreatedAndCalled);
}
