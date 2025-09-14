#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/rx/subject/subject.h>

TEST(RxDebounceTest, Precondition) {

    auto observable = zaf::rx::Observable<int>::Just(1);
    ASSERT_THROW(
        observable.Debounce(std::chrono::milliseconds(100), nullptr), 
        zaf::PreconditionError);
}


TEST(RxDebounceTest, Debounce) {

    zaf::rx::Subject<int> subject;

    auto observable = subject.AsObservable().Debounce(
        std::chrono::milliseconds(10),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::vector<int> values;
    auto sub = observable.Subscribe([&](int value) {
        values.push_back(value);
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    subject.AsObserver().OnNext(3);
    subject.AsObserver().OnNext(4);
    subject.AsObserver().OnNext(5);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    ASSERT_EQ(values, (std::vector<int>{ 2, 5 }));
}


// If there is an error, no more items should be emitted.
TEST(RxDebounceTest, OnError) {

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Debounce(
        std::chrono::milliseconds(10),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::vector<int> values;
    std::exception_ptr captured_error;
    auto sub = observable.Subscribe(
        [&](int value) {
            values.push_back(value);
        },
        [&](std::exception_ptr error) {
            captured_error = error;
        });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnError(zaf::InvalidOperationError{});
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    ASSERT_EQ(values, std::vector<int>{});
    ASSERT_NE(captured_error, nullptr);
}


// The item emission is sent immediately if the source observable completes.
TEST(RxDebounceTest, OnCompletedWithItem) {

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Debounce(
        std::chrono::milliseconds(10),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::vector<int> values;
    auto sub = observable.Subscribe([&](int value) {
        values.push_back(value);
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnCompleted();
    ASSERT_EQ(values, (std::vector<int>{ 2 }));
}


TEST(RxDebounceTest, OnCompletedWithoutItem) {

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Debounce(
        std::chrono::milliseconds(10),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    bool completed = false;
    auto sub = observable.Subscribe(
        [](int) {},
        [](std::exception_ptr) {},
        [&]() {
            completed = true;
        });
    subject.AsObserver().OnCompleted();
    ASSERT_TRUE(completed);
}


TEST(RxDebounceTest, ItemEmissionThread) {

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Debounce(
        std::chrono::milliseconds(10),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::thread::id emission_thread_id;
    auto sub = observable.Subscribe([&](int) {
        emission_thread_id = std::this_thread::get_id();
    });

    subject.AsObserver().OnNext(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    ASSERT_NE(emission_thread_id, std::this_thread::get_id());
}


TEST(RxDebounceTest, ErrorEmissionThread) {

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Debounce(
        std::chrono::milliseconds(10),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::thread::id emission_thread_id;
    auto sub = observable.Subscribe(
        [](int) {},
        [&](std::exception_ptr) {
        emission_thread_id = std::this_thread::get_id();
    });

    subject.AsObserver().OnError(zaf::InvalidOperationError{});
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    ASSERT_EQ(emission_thread_id, std::this_thread::get_id());
}


TEST(RxDebounceTest, CompletionEmissionThread) {

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Debounce(
        std::chrono::milliseconds(10),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::thread::id item_emission_thread_id;
    std::thread::id completion_emission_thread_id;
    auto sub = observable.Subscribe(
        [&](int) {
            item_emission_thread_id = std::this_thread::get_id();
        },
        [](std::exception_ptr) {},
        [&]() {
            completion_emission_thread_id = std::this_thread::get_id();
        });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnCompleted();
    ASSERT_EQ(item_emission_thread_id, std::this_thread::get_id());
    ASSERT_EQ(completion_emission_thread_id, std::this_thread::get_id());
}


TEST(RxDebounceTest, MultipleThreads) {

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Debounce(
        std::chrono::milliseconds(10),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock lock{ mutex };

    std::vector<int> values;
    auto sub = observable.Subscribe([&](int value) {
        values.push_back(value);
        cv.notify_one();
    });

    for (int count = 0; count < 10; ++count) {
        std::thread([&]() {
            for (int value = 0; value < 20; ++value) {
                subject.AsObserver().OnNext(value);
            }
        }).detach();
    }

    cv.wait(lock);
    // Wait for a while to ensure no more items are emitted.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(values.size(), 1);
}