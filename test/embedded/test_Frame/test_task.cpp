#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unity.h>

#include "task.hpp"

using namespace fos;

void setUp(void) {}

void tearDown(void) {}

static bool boolTask1WasCalled = false;
void task1(void *pvParameters) {
    boolTask1WasCalled = true;
    while (true) {
        vTaskDelay(100);
    }
}

void checkIfTaskIsCreatedAndCalled(void) {
    static Task task(task1, "task1");
    Task::delay(10);
    TEST_ASSERT_TRUE(boolTask1WasCalled);
}

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

void app_main(void) {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    vTaskDelay(4000 / portTICK_PERIOD_MS);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(checkIfTaskIsCreatedAndCalled);
    UNITY_END(); // stop unit testing
}
