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

extern void runTaskTests(void);

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

void app_main(void) {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(taskToSuccess);
    runTaskTests();
    UNITY_END(); // stop unit testing
}
