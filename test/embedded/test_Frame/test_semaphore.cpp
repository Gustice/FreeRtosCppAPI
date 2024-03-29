#include "test.hpp"

#include <Semaphore.hpp>
#include <Task.hpp>

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
    Semaphore eut(2, 0);
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
    Semaphore eut(2, 2);
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

static void test1Signal(Semaphore &s) {
    vTaskDelay(SHORT_DELAY);
    s.give();
    while (true) {
        vTaskDelay(SHORT_DELAY);
    };
}

static void testBinarySemTiming() {
    static Semaphore eut;

    TaskT<Semaphore &> signaler(test1Signal, eut, "signal1");
    { // All ok
        TimeTest timer;
        TEST_ASSERT_TRUE(eut.take(100));
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());
    }

    { // Timeout
        TimeTest timer;
        TEST_ASSERT_FALSE(eut.take(20));
        TEST_ASSERT_INT_WITHIN(1, 20, timer.getRunTime());
    }

    { // NoWait timeout
        Semaphore passiveSem;
        TimeTest timer;
        TEST_ASSERT_FALSE(passiveSem.take(0));
        TEST_ASSERT_INT_WITHIN(1, 0, timer.getRunTime());
    }
}

static int syncCount = 0;
static void producerTask(Semaphore &s) {
    while (true) {
        s.give();
        vTaskDelay(SHORT_DELAY);
    };
}
static void consumerTask(Semaphore &s) {
    while (true) {
        s.take();
        syncCount++;
    };
}
static void twoTasksThrowingSemaphores() {
    static Semaphore eut;

    TaskT<Semaphore &> producer(producerTask, eut, "producer");
    TaskT<Semaphore &> consumer(consumerTask, eut, "consumer");
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
