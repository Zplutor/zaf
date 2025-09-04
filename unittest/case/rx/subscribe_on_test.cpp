#include <mutex>
#include <thread>
#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/scheduler/single_thread_scheduler.h>

TEST(RxSubscribeOnTest, Precondition) {

    auto observable = zaf::rx::Observable<int>::Just(0);
    ASSERT_THROW(observable.SubscribeOn(nullptr), zaf::PreconditionError);
}


TEST(RxSubscribeOnTest, SubscribeOn) {

    std::thread::id current_thread = std::this_thread::get_id();
    std::thread::id produce_thread = current_thread;
    std::thread::id on_next_thread = current_thread;
    std::thread::id on_completed_thread = current_thread;

    std::condition_variable cv;
    std::mutex lock;
    std::unique_lock<std::mutex> unique_lock{ lock };

    auto observable = zaf::rx::Observable<int>::Create([&](zaf::rx::Subscriber<int> subscriber) {
        produce_thread = std::this_thread::get_id();
        subscriber.OnNext(0);
        subscriber.OnCompleted();
        return nullptr;
    });

    observable = observable.SubscribeOn(std::make_shared<zaf::rx::SingleThreadScheduler>());
    auto subscription = observable.Subscribe([&](int) {
        on_next_thread = std::this_thread::get_id();
    },
    [&]() {
        on_completed_thread = std::this_thread::get_id();
        std::scoped_lock<std::mutex> lock_guard(lock);
        cv.notify_all();
    });

    cv.wait(unique_lock);

    ASSERT_NE(current_thread, produce_thread);
    ASSERT_NE(current_thread, on_next_thread);
    ASSERT_NE(current_thread, on_completed_thread);
    ASSERT_EQ(produce_thread, on_next_thread);
    ASSERT_EQ(on_next_thread, on_completed_thread);
}