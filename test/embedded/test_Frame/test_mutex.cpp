#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unity.h>

#include "mutex.hpp"
#include "semaphore.hpp"
#include "task.hpp"
#include "teimUtils.hpp"

using namespace fos;

constexpr int SHORT_DELAY = 10;

static void testMutex() {
    Mutex eut;
    // first take fails
    TEST_ASSERT_TRUE(eut.claim(0));
    eut.release();
    
    TEST_ASSERT_TRUE(eut.claim(0));
    TEST_ASSERT_FALSE(eut.claim(0));

    eut.release();
    // eut.release(); // not a good idea
    TEST_ASSERT_TRUE(eut.claim(0));
    // second take after give fails
    TEST_ASSERT_FALSE(eut.claim(0));
}

static Mutex mutex1;
static Semaphore starter1;
static void testClaimer(void *) {
    mutex1.claim();
    starter1.give();
    vTaskDelay(SHORT_DELAY);
    mutex1.release();
    vTaskDelay(SHORT_DELAY);
    mutex1.claim();
    starter1.give();
    while (true) {
        vTaskDelay(1000);
    };
}

static void testMutexTiming() {
    Task claimer(testClaimer,"signal1");
    starter1.take(100);

    { // All ok
        TimeTest timer;
        TEST_ASSERT_TRUE(mutex1.claim(100));
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());
        mutex1.release();
    }
    starter1.take(100);
    { // Timout
        TimeTest timer;
        TEST_ASSERT_FALSE(mutex1.claim(20));
        TEST_ASSERT_INT_WITHIN(1, 20, timer.getRunTime());
    }

    { // NoWeit timeout
        Mutex passiveSem;
        passiveSem.claim(0);
        TimeTest timer;
        TEST_ASSERT_FALSE(passiveSem.claim(0));
        TEST_ASSERT_INT_WITHIN(1, 0, timer.getRunTime());
    }
    claimer.kill();
}

static void testMutexGuard() {
    Mutex mtx;

    {
        MutexGuard eut(mtx,100);
        TEST_ASSERT_TRUE(eut.isActive());
        // already taken
        TEST_ASSERT_FALSE(mtx.claim(0));
    }
    // now free to go
    TEST_ASSERT_TRUE(mtx.claim(0));

    { // Timeout
        TimeTest timer;
        MutexGuard eut(mtx, SHORT_DELAY);
        TEST_ASSERT_FALSE(eut.isActive());
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());
    }

}

void runMutexTests(void) {
    RUN_TEST(testMutex);
    RUN_TEST(testMutexTiming);
    RUN_TEST(testMutexGuard);
}
