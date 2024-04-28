#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/base/error/error.h>
#include <zaf/rx/subject.h>

TEST(RxDoOnTerminatedTest, OnError) {

    int call_sequence{};

    int do_sequence{};
    int sub_sequence{};
    int sub_error_sequence{};

    zaf::Subject<int> subject;
    auto sub = subject.AsObservable().DoOnTerminated([&]() {
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

    zaf::Subject<int> subject;
    auto sub = subject.AsObservable().DoOnTerminated([&]() {
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

    zaf::Subject<int> subject;
    auto observable = subject.AsObservable().DoOnTerminated([&]() {
        ++call_times;
    });

    auto sub1 = observable.Subscribe();
    auto sub2 = observable.Subscribe();

    subject.AsObserver().OnCompleted();
    ASSERT_EQ(call_times, 2);
}