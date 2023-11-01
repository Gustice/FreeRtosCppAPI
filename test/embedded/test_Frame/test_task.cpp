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
static void callableTask() {
    taskWasCalled = true;
    while (true) {
        vTaskDelay(100);
    }
}

void checkIfTaskIsCreatedAndCalled() {
    static Task task(callableTask, "task1");
    Task::delay(50);
    TEST_ASSERT_TRUE(taskWasCalled);
    task.kill();
}

static void callableTask2() {
    taskWasCalled = true;
    while (true) {
        vTaskDelay(100);
    }
}

void checkIfTaskIsKilled() {
    static Task task(callableTask2, "task2");
    auto handle = xTaskGetHandle("task2");
    configASSERT(handle);
    TEST_ASSERT_EQUAL(eTaskState::eReady, eTaskGetState(handle));
    Task::delay(50);
    TEST_ASSERT_TRUE(taskWasCalled);
    task.kill();
    TEST_ASSERT_EQUAL(eTaskState::eDeleted, eTaskGetState(handle));
}

void runTaskTests(void) {
    RUN_TEST(testDelay);
    RUN_TEST(checkIfTaskIsCreatedAndCalled);
    RUN_TEST(checkIfTaskIsKilled);
}
