#include <gtest/gtest.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/rx/subject/subject.h>

TEST(RxThrottleLastTest, PreconditionError) {

    auto observable = zaf::rx::Observable<int>::Just(1);
    ASSERT_THROW(
        observable.ThrottleLast(std::chrono::milliseconds(100), nullptr),
        zaf::PreconditionError);
}


TEST(RxThrottleLastTest, ThrottleLast) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    auto throttle = subject.AsObservable().ThrottleLast(
        std::chrono::milliseconds(50),
        scheduler);

    std::vector<int> values;
    auto sub = throttle.Subscribe([&](int value) {
        values.push_back(value);
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    subject.AsObserver().OnNext(3);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    subject.AsObserver().OnNext(4);
    std::this_thread::sleep_for(std::chrono::milliseconds(70));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(values, (std::vector<int>{3, 4}));
}


TEST(RxThrottleLastTest, CompleteWithPendingValue) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    auto throttle = subject.AsObservable().ThrottleLast(
        std::chrono::milliseconds(50),
        scheduler);

    std::vector<int> values;
    std::chrono::steady_clock::time_point complete_time{};
    auto sub = throttle.Subscribe(
        [&](int value) {
            values.push_back(value);
        },
        [&]() {
            complete_time = std::chrono::steady_clock::now();
        });

    auto start_time = std::chrono::steady_clock::now();
    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnCompleted();

    // Wait for a while to ensure no more value is emitted.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_EQ(values, (std::vector<int>{2}));
    auto duration = complete_time - start_time;
    ASSERT_LT(duration, std::chrono::milliseconds(10));
}


TEST(RxThrottleLastTest, CompleteWithoutPendingValue) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    auto throttle = subject.AsObservable().ThrottleLast(
        std::chrono::milliseconds(50),
        scheduler);

    std::chrono::steady_clock::time_point complete_time{};
    auto sub = throttle.Subscribe(
        [](int value) {},
        [&]() {
            complete_time = std::chrono::steady_clock::now();
        });

    auto start_time = std::chrono::steady_clock::now();
    subject.AsObserver().OnCompleted();
    auto duration = complete_time - start_time;
    ASSERT_LT(duration, std::chrono::milliseconds(10));
}


TEST(RxThrottleLastTest, OnError) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    auto throttle = subject.AsObservable().ThrottleLast(
        std::chrono::milliseconds(50),
        scheduler);

    std::vector<int> values;
    std::exception_ptr error;
    auto sub = throttle.Subscribe(
        [&](int value) {
            values.push_back(value);
        },
        [&](std::exception_ptr e) {
            error = e;
        });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnError(zaf::PreconditionError{});

    // Wait for a while to ensure no more value is emitted.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(values, std::vector<int>{});
    ASSERT_NE(error, nullptr);
}


TEST(RxThrottleLastTest, OnNextThread) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    auto throttle = subject.AsObservable().ThrottleLast(
        std::chrono::milliseconds(50),
        scheduler);

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock lock{ mutex };

    std::thread::id thread_id;
    auto sub = throttle.Subscribe([&](int value) {
        std::lock_guard<std::mutex> lock(mutex);
        thread_id = std::this_thread::get_id();
        cv.notify_one();
    });

    subject.AsObserver().OnNext(1);
    cv.wait(lock);

    ASSERT_NE(thread_id, std::this_thread::get_id());
}


TEST(RxThrottleLastTest, OnErrorThread) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    auto throttle = subject.AsObservable().ThrottleLast(
        std::chrono::milliseconds(50),
        scheduler);

    std::thread::id thread_id;
    auto sub = throttle.Subscribe(
        [](int value) {},
        [&](std::exception_ptr) {
            thread_id = std::this_thread::get_id();
        });
    subject.AsObserver().OnError(zaf::PreconditionError{});
    ASSERT_EQ(thread_id, std::this_thread::get_id());
}


TEST(RxThrottleLastTest, OnCompletedThread) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    auto throttle = subject.AsObservable().ThrottleLast(
        std::chrono::milliseconds(50),
        scheduler);

    std::thread::id on_next_thread_id;
    std::thread::id on_completed_thread_id;
    auto sub = throttle.Subscribe(
        [&](int value) {
            on_next_thread_id = std::this_thread::get_id();
        },
        [&]() {
            on_completed_thread_id = std::this_thread::get_id();
        });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnCompleted();

    ASSERT_EQ(on_next_thread_id, std::this_thread::get_id());
    ASSERT_EQ(on_completed_thread_id, std::this_thread::get_id());
}