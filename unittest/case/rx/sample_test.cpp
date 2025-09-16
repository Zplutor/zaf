#include <gtest/gtest.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/rx/subject/subject.h>
#include <zaf/rx/timer.h>

TEST(RxSampleTest, Precondition) {

    auto observable = zaf::rx::Observable<int>::Just(1);
    ASSERT_THROW(observable.Sample(std::chrono::seconds(1), nullptr), zaf::PreconditionError);
}


TEST(RxSampleTest, Sample) {

    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();
    auto timer = zaf::rx::Timer::Interval(std::chrono::milliseconds(10), scheduler);
    auto sample = timer.Sample(std::chrono::milliseconds(50), scheduler);

    std::vector<std::chrono::steady_clock::time_point> emission_times;
    auto sub = sample.Subscribe([&emission_times](std::size_t value) {
        emission_times.push_back(std::chrono::steady_clock::now());
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    sub->Dispose();

    ASSERT_GE(emission_times.size(), 3);
    ASSERT_LE(emission_times.size(), 4);
    // Each interval between two emissions should be at least 50 milliseconds.
    for (size_t i = 1; i < emission_times.size(); ++i) {
        auto interval = emission_times[i] - emission_times[i - 1];
        ASSERT_GE(interval, std::chrono::milliseconds(50));
    }
}


TEST(RxSampleTest, CompleteWithPendingItem) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::vector<std::chrono::steady_clock::time_point> emission_times;
    auto sub = subject.AsObservable().Sample(std::chrono::milliseconds(50), scheduler).Subscribe(
        [&](int value) {
            {
                std::lock_guard<std::mutex> lock_guard(mutex);
                emission_times.push_back(std::chrono::steady_clock::now());
            }
            cv.notify_one();
        });

    auto start_time = std::chrono::steady_clock::now();
    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnNext(3);
    subject.AsObserver().OnCompleted();
    cv.wait(lock);

    ASSERT_EQ(emission_times.size(), 1);
    auto interval = emission_times[0] - start_time;
    ASSERT_GE(interval, std::chrono::milliseconds(50));
}


TEST(RxSampleTest, CompleteWithoutPendingItem) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    bool has_item_emitted{};
    std::chrono::steady_clock::time_point completed_time;
    auto sub = subject.AsObservable().Sample(std::chrono::milliseconds(50), scheduler).Subscribe(
        [&](int value) {
            std::lock_guard<std::mutex> lock_guard(mutex);
            has_item_emitted = true;
        },
        [&]() {
            std::lock_guard<std::mutex> lock_guard(mutex);
            completed_time = std::chrono::steady_clock::now();
            cv.notify_one();
        });

    auto start_time = std::chrono::steady_clock::now();
    subject.AsObserver().OnCompleted();
    cv.wait(lock);

    ASSERT_FALSE(has_item_emitted);
    auto interval = completed_time - start_time;
    ASSERT_GE(interval, std::chrono::milliseconds(50));
}


TEST(RxSampleTest, Error) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    bool has_item_emitted{};
    bool has_error_emitted{};
    std::chrono::steady_clock::time_point error_time;

    auto sub = subject.AsObservable().Sample(std::chrono::milliseconds(50), scheduler).Subscribe(
        [&](int value) {
            has_item_emitted = true;
        },
        [&](std::exception_ptr) {
            has_error_emitted = true;
            error_time = std::chrono::steady_clock::now();
        });

    auto start_time = std::chrono::steady_clock::now();
    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnNext(3);
    subject.AsObserver().OnError(zaf::PreconditionError{});

    // Wait for a while to ensure that no more item will be emitted.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // The error emission is sent immediately, so the interval should be very short.
    auto interval = error_time - start_time;
    ASSERT_LT(interval, std::chrono::milliseconds(10));
    ASSERT_FALSE(has_item_emitted);
    ASSERT_TRUE(has_error_emitted);
}


TEST(RxSampleTest, OnNextThread) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::thread::id on_next_thread_id;
    auto sub = subject.AsObservable().Sample(std::chrono::milliseconds(50), scheduler).Subscribe(
        [&](int value) {
            on_next_thread_id = std::this_thread::get_id();
            cv.notify_one();
        });

    subject.AsObserver().OnNext(1);
    cv.wait(lock);

    ASSERT_NE(on_next_thread_id, std::this_thread::get_id());
}


TEST(RxSampleTest, OnErrorThread) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    std::thread::id on_error_thread_id;
    auto sub = subject.AsObservable().Sample(std::chrono::milliseconds(50), scheduler).Subscribe(
        [](int) {},
        [&](std::exception_ptr) {
            on_error_thread_id = std::this_thread::get_id();
        });

    subject.AsObserver().OnError(zaf::PreconditionError{});
    ASSERT_EQ(on_error_thread_id, std::this_thread::get_id());
}


TEST(RxSampleTest, OnCompletedThread) {

    zaf::rx::Subject<int> subject;
    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::thread::id on_completed_thread_id;
    auto sub = subject.AsObservable().Sample(std::chrono::milliseconds(50), scheduler).Subscribe(
        [](int) {},
        [&]() {
            on_completed_thread_id = std::this_thread::get_id();
            cv.notify_one();
        });

    subject.AsObserver().OnCompleted();
    cv.wait(lock);
    ASSERT_NE(on_completed_thread_id, std::this_thread::get_id());
}