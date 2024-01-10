#include "test.hpp"

#include <Queue.hpp>
#include <Task.hpp>
#include <string>

using namespace fos;

constexpr int SHORT_DELAY = 10;

struct MessageFrame {
    MessageFrame(int v, std::string msg) : value(v), message(msg) {}
    int value;
    std::string message;
};

static void enqueueThenDequeue(void) {
    Queue<MessageFrame> eut(10);

    auto e = std::make_unique<MessageFrame>(1, "one");
    eut.enqueue(std::move(e));

    auto r = eut.dequeue(100);
    TEST_ASSERT_NOT_NULL(r.get());
    TEST_ASSERT_EQUAL(1, r->value);
    TEST_ASSERT_TRUE(r->message == "one");
}

static void enqueueThenDequeueMultiple(void) {
    Queue<MessageFrame> eut(10);

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

static void enqueueCyclically(void) {
    constexpr int Size = 10;
    Queue<MessageFrame> eut(Size);
    for (size_t i = 0; i < Size * 2; i++) {
        auto e = std::make_unique<MessageFrame>(i, "Some message");
        eut.enqueue(std::move(e));

        auto r = eut.dequeue(100);
        TEST_ASSERT_NOT_NULL(r.get());
        TEST_ASSERT_EQUAL(i, r->value);
    }
    TEST_ASSERT_TRUE(true);
}

static void enqueue_till_overflow(void) {
    Queue<MessageFrame> eut(3);

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

static void enqueueThenDequeueTooMany(void) {
    Queue<MessageFrame> eut(10);

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

static void queue1Gen(Queue<MessageFrame> &q) {
    vTaskDelay(SHORT_DELAY);
    auto e = std::make_unique<MessageFrame>(0, "msg");
    q.enqueue(std::move(e));
    while (true) {
        vTaskDelay(SHORT_DELAY);
    };
}

static void testTiming() {
    static Queue<MessageFrame> eut(8);
    TaskT<Queue<MessageFrame> &> signaler(queue1Gen, eut, "queue1Gen");
    { // All ok
        TimeTest timer;
        auto r = eut.dequeue(100);
        TEST_ASSERT_NOT_NULL(r.get());
        TEST_ASSERT_INT_WITHIN(1, SHORT_DELAY, timer.getRunTime());
    }

    { // Timeout
        TimeTest timer;
        auto r = eut.dequeue(20);
        TEST_ASSERT_NULL(r.get());
        TEST_ASSERT_INT_WITHIN(1, 20, timer.getRunTime());
    }
}

static void senderTask(Queue<MessageFrame> &q) {
    size_t i = 0;
    while (q.isActive()) {
        auto e = std::make_unique<MessageFrame>(i++, "Some message");
        q.enqueue(std::move(e));
        vTaskDelay(1);
    }
}

static void enqueueSeriesOfMessages(void) {
    constexpr int Size = 10;
    Queue<MessageFrame> eut(Size);
    TaskT<Queue<MessageFrame> &> sender(senderTask, eut, "sender");
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
    RUN_TEST(enqueueThenDequeueTooMany);
    RUN_TEST(testTiming);
    RUN_TEST(enqueueSeriesOfMessages);
}
