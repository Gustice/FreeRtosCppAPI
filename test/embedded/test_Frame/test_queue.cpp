#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unity.h>

#include "queue.hpp"
#include "task.hpp"
#include "teimUtils.hpp"

using namespace fos;

constexpr int SHORT_DELAY = 10;

struct MessageFrame {
    MessageFrame(int v, std::string msg) : value(v), message(msg) {}
    int value;
    std::string message;
};

void enqueueThenDequeue(void) {
    MessageQueue<MessageFrame> eut(10);

    auto e = std::make_unique<MessageFrame>(1, "one");
    eut.enqueue(std::move(e));

    auto r = eut.dequeue(100);
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(1, r->value);
    TEST_ASSERT_TRUE(r->message == "one");
}

void enqueueThenDequeueMultiple(void) {
    MessageQueue<MessageFrame> eut(10);

    auto e = std::make_unique<MessageFrame>(1, "one");
    eut.enqueue(std::move(e));
    e = std::make_unique<MessageFrame>(2, "two");
    eut.enqueue(std::move(e));
    e = std::make_unique<MessageFrame>(3, "three");
    eut.enqueue(std::move(e));

    auto r = eut.dequeue(100);
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(1, r->value);
    TEST_ASSERT_TRUE(r->message == "one");
    r = eut.dequeue(100);
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(2, r->value);
    TEST_ASSERT_TRUE(r->message == "two");
    r = eut.dequeue(100);
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(3, r->value);
    TEST_ASSERT_TRUE(r->message == "three");
}

void enqueueCyclically(void) {
    constexpr int Size = 10;
    MessageQueue<MessageFrame> eut(Size);
    for (size_t i = 0; i < Size * 2; i++) {
        auto e = std::make_unique<MessageFrame>(i, "Some message");
        eut.enqueue(std::move(e));

        auto r = eut.dequeue(100);
        TEST_ASSERT_NOT_NULL(r.get());
        TEST_ASSERT_EQUAL(i, r->value);
    }
    TEST_ASSERT_TRUE(true);
}

void enqueue_till_overflow(void) {
    MessageQueue<MessageFrame> eut(3);

    auto e = std::make_unique<MessageFrame>(1, "one");
    auto r = eut.enqueue(std::move(e));
    TEST_ASSERT_EQUAL(nullptr, r.get());

    e = std::make_unique<MessageFrame>(2, "two");
    r = eut.enqueue(std::move(e));
    TEST_ASSERT_EQUAL(nullptr, r.get());

    e = std::make_unique<MessageFrame>(3, "three");
    r = eut.enqueue(std::move(e));
    TEST_ASSERT_EQUAL(nullptr, r.get());

    e = std::make_unique<MessageFrame>(4, "four");
    auto pE = e.get();
    r = eut.enqueue(std::move(e));
    TEST_ASSERT_NOT_EQUAL(nullptr, r.get());
    // also return old element ... 
    TEST_ASSERT_EQUAL(pE, r.get());
}

void enqueue_then_dequeue_to_many(void) {
    MessageQueue<MessageFrame> eut(10);

    auto e = std::make_unique<MessageFrame>(1, "one");
    eut.enqueue(std::move(e));
    e = std::make_unique<MessageFrame>(2, "two");
    eut.enqueue(std::move(e));

    auto r = eut.dequeue(100);
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(1, r->value);
    r = eut.dequeue(100);
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(2, r->value);
    r = eut.dequeue(0); // no waiting
    TEST_ASSERT_EQUAL(nullptr, r.get());
}

static MessageQueue<MessageFrame> queue1(8);
static void queue1Gen(void *) {
    vTaskDelay(SHORT_DELAY);
    auto e = std::make_unique<MessageFrame>(0, "msg");
    queue1.enqueue(std::move(e));
    while (true) {
        vTaskDelay(1000);
    };
}

static void testTiming() {
    Task signaler(queue1Gen, "signal1");
    { // All ok
        TimeTest timer;
        auto r = queue1.dequeue(100);
        TEST_ASSERT_NOT_NULL(r.get());
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());
    }

    { // Timout
        TimeTest timer;
        auto r = queue1.dequeue(20);
        TEST_ASSERT_NULL(r.get());
        TEST_ASSERT_INT_WITHIN(1, 20, timer.getRunTime());
    }
}

static void senderTask(void *arg) {
    auto &q = *static_cast<MessageQueue<MessageFrame> *>(arg);
    size_t i = 0;
    while (q.isActive()) {
        auto e = std::make_unique<MessageFrame>(i++, "Some message");
        q.enqueue(std::move(e));
        vTaskDelay(1);
    }
}

void enqueueSeriesOfMessages(void) {
    constexpr int Size = 10;
    MessageQueue<MessageFrame> eut(Size);
    Task signaler(senderTask, eut, "displayTask");
    for (size_t i = 0; i < Size * 2; i++) {
        auto r = eut.dequeue(100);
        TEST_ASSERT_NOT_NULL(r.get());
        TEST_ASSERT_EQUAL(i, r->value);
    }
    eut.signalClose();
    TEST_ASSERT_TRUE(true);
}


void runQueueTests(void) {
    RUN_TEST(enqueueThenDequeue);
    RUN_TEST(enqueueThenDequeueMultiple);
    RUN_TEST(enqueueCyclically);
    RUN_TEST(enqueue_till_overflow);
    RUN_TEST(enqueue_then_dequeue_to_many);
    RUN_TEST(testTiming);
    RUN_TEST(enqueueSeriesOfMessages);
}




