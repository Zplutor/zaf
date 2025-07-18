#include <mutex>
#include <gtest/gtest.h>
#include <zaf/base/error/error.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_host.h>

using namespace zaf;

TEST(RxDoOnTerminatedTest, OnError) {

    int call_sequence{};

    int do_sequence{};
    int sub_sequence{};
    int sub_error_sequence{};

    zaf::rx::Subject<int> subject;
    auto sub = subject.AsObservable().DoOnTerminate([&]() {
        do_sequence = ++call_sequence;
    })
    .Subscribe([&](int value) {
        sub_sequence = ++call_sequence;
    },
    [&](const std::exception_ptr& error) {
        sub_error_sequence = ++call_sequence;
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnError(std::make_exception_ptr(0));

    ASSERT_EQ(sub_sequence, 1);
    ASSERT_EQ(do_sequence, 2);
    ASSERT_EQ(sub_error_sequence, 3);
}


TEST(RxDoOnTerminatedTest, OnCompleted) {

    int call_sequence{};

    int do_sequence{};
    int sub_sequence{};
    int sub_completed_sequence{};

    zaf::rx::Subject<int> subject;
    auto sub = subject.AsObservable().DoOnTerminate([&]() {
        do_sequence = ++call_sequence;
    })
    .Subscribe([&](int value) {
        sub_sequence = ++call_sequence;
    },
    [&]() {
        sub_completed_sequence = ++call_sequence;
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnCompleted();

    ASSERT_EQ(sub_sequence, 1);
    ASSERT_EQ(do_sequence, 2);
    ASSERT_EQ(sub_completed_sequence, 3);
}


TEST(RxDoOnTerminatedTest, SubscribeMultipleTimes) {

    int call_times{};

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().DoOnTerminate([&]() {
        ++call_times;
    });

    auto sub1 = observable.Subscribe();
    auto sub2 = observable.Subscribe();

    subject.AsObserver().OnCompleted();
    ASSERT_EQ(call_times, 2);
}


/**
In this test case, we destroy the notification source and the subscription in DoOnTerminate(), in
such case, the producer of the DoOnTerminate() itself is destroyed as well, causing an access 
violation exception when it tries to deliver the OnCompleted notification. We use this case to make 
sure it has been fixed.
*/
TEST(RxDoOnTerminatedTest, DestroySource) {

    std::optional<rx::Subject<int>> subject;
    subject.emplace();

    std::optional<rx::SubscriptionHost> sub_host;
    sub_host.emplace();

    std::condition_variable cv;
    std::mutex lock;
    std::unique_lock<std::mutex> unique_lock{ lock };

    sub_host->Subscriptions() += subject->AsObservable()
        .ObserveOn(rx::Scheduler::CreateOnSingleThread())
        .DoOnTerminate([&]() {

            subject.reset();
            sub_host.reset();

            std::scoped_lock<std::mutex> lock_guard(lock);
            cv.notify_all();
        })
        .Subscribe();

    subject->AsObserver().OnCompleted();
    cv.wait(unique_lock);
}