#include <unity.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void setUp(void) {}

void tearDown(void) {}

void test_to_pass(void) {
    TEST_ASSERT_TRUE(true);
}

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

void app_main(void) {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_to_pass);
    UNITY_END(); // stop unit testing
}
