#include <mutex>
#include <thread>
#include <gtest/gtest.h>
#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/rx/subject/subject.h>

TEST(RxObserveOnTest, ObserveOn) {

    auto current_thread = std::this_thread::get_id();
    std::thread::id on_next_thread = current_thread;
    std::thread::id on_error_thread = current_thread;
    std::thread::id on_completed_thread = current_thread;

    std::condition_variable cv;
    std::mutex lock;
    std::unique_lock<std::mutex> unique_lock{ lock };

    //OnNext and OnCompleted
    {
        zaf::rx::Subject<int> subject;
        auto observable = subject.AsObservable().ObserveOn(
            std::make_shared<zaf::rx::SingleThreadScheduler>());

        auto subscription = observable.Subscribe([&](int) {
            on_next_thread = std::this_thread::get_id();
        },
        [&]() {
            on_completed_thread = std::this_thread::get_id();
            std::scoped_lock<std::mutex> lock_guard(lock);
            cv.notify_all();
        });

        subject.AsObserver().OnNext(3);
        subject.AsObserver().OnCompleted();
        cv.wait(unique_lock);
    }

    //OnError
    {
        zaf::rx::Subject<int> subject;
        auto observable = subject.AsObservable().ObserveOn(
            std::make_shared<zaf::rx::SingleThreadScheduler>());

        auto subscription = observable.Subscribe([](int) { },
        [&](const std::exception_ptr&) {
            on_error_thread = std::this_thread::get_id();
            std::scoped_lock<std::mutex> lock_guard(lock);
            cv.notify_all();
        });

        subject.AsObserver().OnError(std::make_exception_ptr(8));
        cv.wait(unique_lock);
    }

    ASSERT_NE(current_thread, on_next_thread);
    ASSERT_NE(current_thread, on_error_thread);
    ASSERT_NE(current_thread, on_completed_thread);
}