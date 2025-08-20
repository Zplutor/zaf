#include <mutex>
#include <gtest/gtest.h>
#include <zaf/base/error/error.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/rx/subject/subject.h>
#include <zaf/rx/disposable_host.h>

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

    std::optional<rx::DisposableHost> sub_host;
    sub_host.emplace();

    std::condition_variable cv;
    std::mutex lock;
    std::unique_lock<std::mutex> unique_lock{ lock };

    sub_host->Disposables() += subject->AsObservable()
        .ObserveOn(std::make_shared<rx::SingleThreadScheduler>())
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


// Exception thrown in DoOnTerminate() should be propagated to the OnError handler of the 
// downstream observer.
TEST(RxDoOnTerminatedTest, Throw) {

    zaf::rx::Subject<int> subject;

    // Triggered by OnCompleted.
    {
        bool on_error_called{};
        bool on_completed_called{};
        auto sub = subject.AsObservable().DoOnTerminate([&]() {
            throw zaf::InvalidOperationError();
        })
        .Subscribe([](int) {
        },
        [&](const std::exception_ptr& error) {
            on_error_called = true;
        },
        [&]() {
            on_completed_called = true;
        });

        subject.AsObserver().OnCompleted();
        ASSERT_TRUE(on_error_called);
        ASSERT_FALSE(on_completed_called);
    }

    // Triggered by OnError.
    {
        std::string error_message;
        bool on_completed_called{};
        auto sub = subject.AsObservable().DoOnTerminate([&]() {
            throw zaf::InvalidOperationError("DoOnTerminate");
        })
        .Subscribe([](int) {
        },
        [&](const std::exception_ptr& error) {
            try {
                std::rethrow_exception(error);
            }
            catch (const zaf::InvalidOperationError& e) {
                error_message = e.what();
            }
        },
        [&]() {
            on_completed_called = true;
        });
        subject.AsObserver().OnError(zaf::InvalidOperationError("OnError"));
        ASSERT_EQ(error_message, "DoOnTerminate");
        ASSERT_FALSE(on_completed_called);
    }
}