#include "test.hpp"

void setUp(void) {}

void tearDown(void) {}

void taskToSuccess() {
    TEST_ASSERT_TRUE(true);
}

extern void runTaskTests(void);
extern void runSemaphoreTests(void);
extern void runQueueTests(void);
extern void runMutexTests(void);

void runTests() {
    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(taskToSuccess);
    runTaskTests();
    runSemaphoreTests();
    runQueueTests();
    runMutexTests();
    UNITY_END(); // stop unit testing
}

#ifdef ARDUINO_ARCH_STM32

void setup() {
    delay(2000);
    runTests();
}

void loop() {
    // Empty. Things are done in Tasks.
}
#elif ESP_PLATFORM

extern "C" { // This switch allows the ROS C-implementation to find this main
void app_main(void);
}

void app_main(void) {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    runTests();
}
#endif
