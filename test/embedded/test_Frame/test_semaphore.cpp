#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unity.h>

#include "semaphore.hpp"
#include "task.hpp"
#include "teimUtils.hpp"

using namespace fos;

constexpr int SHORT_DELAY = 10;

static void testBinarySem() {
    Semaphore eut;
    // first take fails
    TEST_ASSERT_FALSE(eut.take(0));
    eut.give();
    // first take after give fails
    TEST_ASSERT_TRUE(eut.take(0));
    TEST_ASSERT_FALSE(eut.take(0));

    eut.give();
    eut.give();
    TEST_ASSERT_TRUE(eut.take(0));
    // second take after give fails
    TEST_ASSERT_FALSE(eut.take(0));
}

static void testCountingSem() {
    Semaphore eut(2,0);
    // first take fails
    TEST_ASSERT_FALSE(eut.take(0));
    eut.give();
    // first take after give fails
    TEST_ASSERT_TRUE(eut.take(0));
    TEST_ASSERT_FALSE(eut.take(0));

    eut.give();
    eut.give();
    TEST_ASSERT_TRUE(eut.take(0));
    TEST_ASSERT_TRUE(eut.take(0));
    // third take after give fails
    TEST_ASSERT_FALSE(eut.take(0));

    eut.give();
    eut.give();
    eut.give();
    TEST_ASSERT_TRUE(eut.take(0));
    TEST_ASSERT_TRUE(eut.take(0));
    // still third take after give fails
    TEST_ASSERT_FALSE(eut.take(0));
}

static void testCountingSemInit() {
    Semaphore eut(2,2);
    TEST_ASSERT_TRUE(eut.take(0));
    TEST_ASSERT_TRUE(eut.take(0));
    // third take fails
    TEST_ASSERT_FALSE(eut.take(0));

    eut.give();
    // first take after give fails
    TEST_ASSERT_TRUE(eut.take(0));
    TEST_ASSERT_FALSE(eut.take(0));

    // ...
}

static Semaphore binSem1;
static void test1Signal(void *) {
    vTaskDelay(SHORT_DELAY);
    binSem1.give();
    while (true) {
        vTaskDelay(1000);
    };
}

static void testBinarySemTiming() {
    Task signaler(test1Signal, "signal1");
    { // All ok
        TimeTest timer;
        TEST_ASSERT_TRUE(binSem1.take(100));
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());
    }

    { // Timout
        TimeTest timer;
        TEST_ASSERT_FALSE(binSem1.take(20));
        TEST_ASSERT_INT_WITHIN(1, 20, timer.getRunTime());
    }

    { // NoWeit timeout
        Semaphore passiveSem;
        TimeTest timer;
        TEST_ASSERT_FALSE(passiveSem.take(0));
        TEST_ASSERT_INT_WITHIN(1, 0, timer.getRunTime());
    }
}

static Semaphore syncSem;
static int syncCount = 0;
static void producerTask(void *) {
    while (true) {
        syncSem.give();
        vTaskDelay(SHORT_DELAY);
    };
}
static void consumerTask(void *) {
    while (true) {
        syncSem.take();
        syncCount++;
    };
}
static void twoTasksThrowingSemaphores() {
    Task producer(producerTask, "producer");
    Task consumer(consumerTask, "consumer");
    vTaskDelay(5 * SHORT_DELAY);
    TEST_ASSERT_INT_WITHIN(1, 5, syncCount);
}

void runSemaphoreTests(void) {
    RUN_TEST(testBinarySem);
    RUN_TEST(testCountingSem);
    RUN_TEST(testCountingSemInit);
    RUN_TEST(testBinarySemTiming);
    RUN_TEST(twoTasksThrowingSemaphores);
}
