#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unity.h>
#include "esp_log.h"

#include "task.hpp"
#include "semaphore.hpp"

using namespace fos;

static Semaphore binSem1;
static void test1Signal(void *) {
    vTaskDelay(20);
    binSem1.give();
    while (true) {
        vTaskDelay(20);
    };
}

static void testBinarySem() {
    { // All ok
        Task signaler(test1Signal, "signal");
        TickType_t start = xTaskGetTickCount ();
        auto ret = binSem1.take(100);
        TEST_ASSERT_TRUE(ret);
        TickType_t end = xTaskGetTickCount ();
        TEST_ASSERT_INT_WITHIN (1, 20, (end-start));
    }

    { // Timout
        TickType_t start = xTaskGetTickCount ();
        auto ret = binSem1.take(40);
        TEST_ASSERT_FALSE(ret);
        TickType_t end = xTaskGetTickCount ();
        TEST_ASSERT_INT_WITHIN (1, 40, (end-start));
    }
}

void runSemaphoreTests(void) {
    RUN_TEST(testBinarySem);
}
