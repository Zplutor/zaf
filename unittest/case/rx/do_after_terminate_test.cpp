#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/single.h>
#include <zaf/rx/subject.h>

TEST(RxDoAfterTerminateTest, OnError) {

    int call_sequence{};
    int after_terminate_sequence{};
    int on_next_sequence{};
    int on_error_sequence{};

    zaf::rx::Subject<int> subject;
    auto sub = subject.AsObservable().DoAfterTerminate([&]() {
        after_terminate_sequence = ++call_sequence;
    })
    .Subscribe([&](int value) {
        on_next_sequence = ++call_sequence;
    },
    [&](const std::exception_ptr& error) {
        on_error_sequence = ++call_sequence;
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnError(std::string("error"));

    ASSERT_EQ(on_next_sequence, 1);
    ASSERT_EQ(on_error_sequence, 2);
    ASSERT_EQ(after_terminate_sequence, 3);
}


TEST(RxDoAfterTerminateTest, OnCompleted) {

    int call_sequence{};
    int after_terminate_sequence{};
    int on_next_sequence{};
    int on_completed_sequence{};

    zaf::rx::Subject<int> subject;
    auto sub = subject.AsObservable().DoAfterTerminate([&]() {
        after_terminate_sequence = ++call_sequence;
    })
    .Subscribe([&](int value) {
        on_next_sequence = ++call_sequence;
    },
    [&]() {
        on_completed_sequence = ++call_sequence;
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnCompleted();

    ASSERT_EQ(on_next_sequence, 1);
    ASSERT_EQ(on_completed_sequence, 2);
    ASSERT_EQ(after_terminate_sequence, 3);
}


TEST(RxDoAfterTerminateTest, DoOnTerminate) {

    int call_sequence{};
    int on_terminate1_sequence{};
    int on_terminate2_sequence{};
    int after_terminate_sequence{};
    int on_next_sequence{};
    int on_completed_sequence{};

    zaf::rx::Subject<int> subject;
    auto sub = subject.AsObservable().DoOnTerminate([&]() {
        on_terminate1_sequence = ++call_sequence;
    })
    .DoAfterTerminate([&]() {
        after_terminate_sequence = ++call_sequence;
    })
    // This DoOnTerminate will be called before DoAfterTerminate even it is placed after it in the 
    // chain.
    .DoOnTerminate([&]() {
        on_terminate2_sequence = ++call_sequence;
    })
    .Subscribe([&](int value) {
        on_next_sequence = ++call_sequence;
    },
    [&]() {
        on_completed_sequence = ++call_sequence;
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnCompleted();

    ASSERT_EQ(on_next_sequence, 1);
    ASSERT_EQ(on_terminate1_sequence, 2);
    ASSERT_EQ(on_terminate2_sequence, 3);
    ASSERT_EQ(on_completed_sequence, 4);
    ASSERT_EQ(after_terminate_sequence, 5);
}


TEST(RxDoAfterTerminateTest, Unsubscribe) {

    bool after_terminate_called{};

    zaf::rx::Subject<int> subject;
    auto sub = subject.AsObservable().DoAfterTerminate([&]() {
        after_terminate_called = true;
    })
    .Subscribe();

    sub.Unsubscribe();
    //Unsubscribe should not call DoAfterTerminate.
    ASSERT_EQ(after_terminate_called, false);
}


TEST(RxDoAfterTerminateTest, Single) {

    //OnSuccess
    {
        int call_sequence{};
        int after_terminate_sequence{};
        int on_success_sequence{};

        auto single = zaf::rx::Single<int>::Just(1);
        auto sub = single.DoAfterTerminate([&]() {
            after_terminate_sequence = ++call_sequence;
        })
        .Subscribe([&](int value) {
            on_success_sequence = ++call_sequence;
        });
        ASSERT_EQ(on_success_sequence, 1);
        ASSERT_EQ(after_terminate_sequence, 2);
    }

    //OnError
    {
        int call_sequence{};
        int after_terminate_sequence{};
        int on_error_sequence{};
        auto single = zaf::rx::Single<int>::Throw(zaf::InvalidOperationError{ "error" });
        auto sub = single.DoAfterTerminate([&]() {
            after_terminate_sequence = ++call_sequence;
        })
        .Subscribe(
            [](int) {},
            [&](const std::exception_ptr&) {
                on_error_sequence = ++call_sequence;
            }
        );
        ASSERT_EQ(on_error_sequence, 1);
        ASSERT_EQ(after_terminate_sequence, 2);
    }
}
