#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <array>
#include <unity.h>

#include "task.hpp"

using namespace fos;

static void testDelay() {
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

static void checkIfTaskIsCreatedAndCalled() {
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

static void checkIfTaskIsKilled() {
    static Task task(callableTask2, "task2");
    auto handle = xTaskGetHandle("task2");
    configASSERT(handle);
    TEST_ASSERT_EQUAL(eTaskState::eReady, eTaskGetState(handle));
    Task::delay(50);
    TEST_ASSERT_TRUE(taskWasCalled);
    task.kill();
    TEST_ASSERT_EQUAL(eTaskState::eDeleted, eTaskGetState(handle));
}

static std::array<int, 2> values;

static void oneOfMultiple(void *i) {
    static std::array<int, 2> c{1, -1};
    int idx = *reinterpret_cast<int *>(i);

    for (size_t i = 0; i < 10; i++) {
        values.at(idx) += c.at(idx);
    }

    while (true) {
        vTaskDelay(100);
    }
}

static void checkIfTwoTasksDoNotInterfere() {
    int param1 = 0;
    Task task1(oneOfMultiple, param1, "pTask1");
    int param2 = 1;
    Task task2(oneOfMultiple, param2, "pTask2");

    Task::delay(50);

    task1.kill();
    task2.kill();

    TEST_ASSERT_EQUAL(10, values[0]);
    TEST_ASSERT_EQUAL(-10, values[1]);
}

void runTaskTests(void) {
    RUN_TEST(testDelay);
    RUN_TEST(checkIfTaskIsCreatedAndCalled);
    RUN_TEST(checkIfTaskIsKilled);
    RUN_TEST(checkIfTwoTasksDoNotInterfere);
}
