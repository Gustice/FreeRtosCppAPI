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

static void testerTask(void *) {
    runTests();
    while (true) {
        vTaskDelay(1000);
    }
}

#ifdef ARDUINO_ARCH_STM32

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        // wait for serial port to connect. Needed for native USB
    }
    delay(500);

    xTaskCreate(testerTask, "Tester", 0x1000, nullptr, 1, nullptr);

    // start scheduler
    vTaskStartScheduler();

    Serial.println("Insufficient RAM");
    while (1) // error if it ends up here
        ;
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
    xTaskCreate(testerTask, "Tester", 0x4000, nullptr, 1, nullptr);
    while (true) {
        vTaskDelay(1000);
    }
}

#endif
