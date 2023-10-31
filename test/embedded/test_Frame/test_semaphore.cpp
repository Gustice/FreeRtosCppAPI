#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unity.h>

#include "semaphore.hpp"
#include "task.hpp"
#include "teimUtils.hpp"

using namespace fos;

constexpr int SHORT_DELAY = 10;

static Semaphore binSem1;
static void test1Signal(void *) {
    vTaskDelay(SHORT_DELAY);
    binSem1.give();
    while (true) {
        vTaskDelay(1000);
    };
}

static void testBinarySem() {
    { // All ok
        Task signaler(test1Signal, "signal1");
        TimeTest timer;
        TEST_ASSERT_TRUE(binSem1.take(100));
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());
    }

    { // Timout
        TimeTest timer;
        TEST_ASSERT_FALSE(binSem1.take(20));
        TEST_ASSERT_INT_WITHIN(1, 20, timer.getRunTime());
    }
}

static Semaphore cntSem1(2, 0);
static void test2Signal(void *) {
    vTaskDelay(SHORT_DELAY);
    cntSem1.give();
    cntSem1.give();
    while (true) {
        vTaskDelay(1000);
    };
}

static void testCountingSem() {
    { // All ok
        Task signaler(test2Signal, "signal2");
        TimeTest timer;
        TEST_ASSERT_TRUE(cntSem1.take(100));
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());

        // can take second semaphore instantly
        TEST_ASSERT_TRUE(cntSem1.take(100));
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());
    }

    { // Timout
        TimeTest timer;
        TEST_ASSERT_FALSE(cntSem1.take(20));
        TEST_ASSERT_INT_WITHIN(1, 20, timer.getRunTime());
    }
}

static Semaphore cntSem2(2, 2);
static void test3Signal(void *) {
    vTaskDelay(SHORT_DELAY);
    cntSem2.give();
    cntSem2.give();
    while (true) {
        vTaskDelay(1000);
    };
}

static void testCountingSemWithInit() {
    { // All ok
        Task signaler(test3Signal, "signal3");
        TimeTest timer;
        // First two semaphores instantly
        TEST_ASSERT_TRUE(cntSem2.take(0));
        TEST_ASSERT_TRUE(cntSem2.take(0));
        // then after 20 ms
        TEST_ASSERT_TRUE(cntSem2.take(100));
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());

        // can take second semaphore instantly
        TEST_ASSERT_TRUE(cntSem2.take(100));
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());
    }

    { // Timout
        TimeTest timer;
        TEST_ASSERT_FALSE(cntSem2.take(20));
        TEST_ASSERT_INT_WITHIN(1, 20, timer.getRunTime());
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

static Semaphore passiveSem;

static void testNoWaitOnSemaphore() {
    { // Timout
        TimeTest timer;
        TEST_ASSERT_FALSE(binSem1.take(0));
        TEST_ASSERT_INT_WITHIN(1, 0, timer.getRunTime());
    }
}

void runSemaphoreTests(void) {
    RUN_TEST(testBinarySem);
    RUN_TEST(testCountingSem);
    RUN_TEST(testCountingSemWithInit);
    RUN_TEST(twoTasksThrowingSemaphores);
    RUN_TEST(testNoWaitOnSemaphore);
}
