#include <mutex>
#include <gtest/gtest.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/rx/timer.h>

TEST(RxTimerTest, OneShotTimer) {

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    zaf::rx::Observable<std::size_t> timer = zaf::rx::Timer::Once(
        std::chrono::milliseconds(50),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::vector<std::size_t> emitted_values;
    bool on_error_called{};
    bool on_completed_called{};

    auto sub = timer.Subscribe([&](std::size_t value) {
        std::lock_guard<std::mutex> lock_guard(mutex);
        emitted_values.push_back(value);
    }, 
    [&](std::exception_ptr) {
        std::lock_guard<std::mutex> lock_guard(mutex);
        on_error_called = true;
    },
    [&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex);
            on_completed_called = true;
        }
        cv.notify_one();
    });

    cv.wait(lock, [&]() {
        return on_completed_called;
    });

    ASSERT_EQ(emitted_values, std::vector<std::size_t>{ 0 });
    ASSERT_FALSE(on_error_called);
    ASSERT_TRUE(on_completed_called);
    // Wait for the timer to be disposed.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(sub->IsDisposed());
}


TEST(RxTimerTest, DelayRepeatingTimer) {

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    auto timer = zaf::rx::Timer::DelayInterval(
        std::chrono::milliseconds(50),
        std::chrono::milliseconds(20),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::vector<std::size_t> emitted_values;
    bool on_error_called{};
    bool on_completed_called{};
    auto sub = timer.Subscribe([&](std::size_t value) {
        {
            std::lock_guard<std::mutex> lock_guard(mutex);
            emitted_values.push_back(value);
        }
        cv.notify_one();
    }, 
    [&](std::exception_ptr) {
        std::lock_guard<std::mutex> lock_guard(mutex);
        on_error_called = true;
    },
    [&]() {
        std::lock_guard<std::mutex> lock_guard(mutex);
        on_completed_called = true;
    });
    cv.wait(lock, [&]() {
        return emitted_values.size() >= 3;
    });
    sub->Dispose();

    ASSERT_EQ(emitted_values, (std::vector<std::size_t>{ 0, 1, 2 }));
    ASSERT_FALSE(on_error_called);
    ASSERT_FALSE(on_completed_called);
}


TEST(RxTimerTest, RepeatingTimer) {

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    auto timer = zaf::rx::Timer::Interval(
        std::chrono::milliseconds(50),
        std::make_shared<zaf::rx::SingleThreadScheduler>());

    std::vector<std::size_t> emitted_values;
    bool on_error_called{};
    bool on_completed_called{};
    auto sub = timer.Subscribe([&](std::size_t value) {
        {
            std::lock_guard<std::mutex> lock_guard(mutex);
            emitted_values.push_back(value);
        }
        cv.notify_one();
    },
    [&](std::exception_ptr) {
        std::lock_guard<std::mutex> lock_guard(mutex);
        on_error_called = true;
    },
    [&]() {
        std::lock_guard<std::mutex> lock_guard(mutex);
        on_completed_called = true;
    });
    cv.wait(lock, [&]() {
        return emitted_values.size() >= 3;
    });
    sub->Dispose();

    ASSERT_EQ(emitted_values, (std::vector<std::size_t>{ 0, 1, 2 }));
    ASSERT_FALSE(on_error_called);
    ASSERT_FALSE(on_completed_called);
}