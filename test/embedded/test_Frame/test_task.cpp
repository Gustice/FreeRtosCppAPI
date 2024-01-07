#include "test.hpp"

#include <Task.hpp>
#include <array>

using namespace fos;

static void testDelay() {
    TickType_t start = xTaskGetTickCount();
    Task::delayMs(5 * portTICK_PERIOD_MS);
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
    Task::delayMs(50);
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
#if (INCLUDE_xTaskGetHandle == 1)
    static Task task(callableTask2, "task2");
    auto handle = xTaskGetHandle("task2");
    configASSERT(handle);
    TEST_ASSERT_EQUAL(eTaskState::eReady, eTaskGetState(handle));
    Task::delayMs(50);
    TEST_ASSERT_TRUE(taskWasCalled);
    task.kill();
    TEST_ASSERT_EQUAL(eTaskState::eDeleted, eTaskGetState(handle));
#endif
}

static std::array<int, 2> values;

static void oneOfMultiple(int idx) {
    static std::array<int, 2> c{1, -1};

    for (size_t i = 0; i < 10; i++) {
        values.at(idx) += c.at(idx);
    }

    while (true) {
        vTaskDelay(100);
    }
}

static void checkIfTwoTasksDoNotInterfere() {
    int param1 = 0;
    TaskT<int> task1(oneOfMultiple, param1, "pTask1");
    int param2 = 1;
    TaskT<int> task2(oneOfMultiple, param2, "pTask2");

    Task::delayMs(50);

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
