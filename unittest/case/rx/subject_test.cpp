#include <gtest/gtest.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/subject/subject.h>
#include <zaf/rx/internal/subject/subject_core.h>

static_assert(!std::is_copy_assignable_v<zaf::rx::Subject<int>>);
static_assert(!std::is_copy_constructible_v<zaf::rx::Subject<int>>);
static_assert(std::is_move_assignable_v<zaf::rx::Subject<int>>);
static_assert(std::is_move_constructible_v<zaf::rx::Subject<int>>);


TEST(RxSubjectTest, OnCompleted) {

    zaf::rx::Subject<int> subject;

    std::vector<int> values;
    int on_error_count{};
    int on_completed_count{};
    auto sub = subject.AsObservable().Subscribe([&values](int value) {
        values.push_back(value);
    }, 
    [&](std::exception_ptr) {
        on_error_count++;
    }, 
    [&]() {
        on_completed_count++;
    });

    subject.AsObserver().OnNext(8);
    subject.AsObserver().OnNext(5);
    subject.AsObserver().OnNext(0);
    subject.AsObserver().OnNext(3);
    subject.AsObserver().OnCompleted();

    std::vector<int> expected{ 8, 5, 0, 3 };
    ASSERT_EQ(values, expected);
    ASSERT_EQ(on_error_count, 0);
    ASSERT_EQ(on_completed_count, 1);

    // Any emission will be ignored after termination.
    subject.AsObserver().OnNext(4);
    subject.AsObserver().OnError(std::make_exception_ptr(std::runtime_error("")));
    subject.AsObserver().OnCompleted();
    ASSERT_EQ(values, expected);
    ASSERT_EQ(on_error_count, 0);
    ASSERT_EQ(on_completed_count, 1);
}


TEST(RxSubjectTest, SubscribeAfterOnCompleted) {

    zaf::rx::Subject<int> subject;
    subject.AsObserver().OnNext(18);
    subject.AsObserver().OnCompleted();

    bool on_next_called{};
    bool on_error_called{};
    bool on_completed_called{};

    // Subscribing to a terminated subject will receive OnCompleted immediately.
    auto sub = subject.AsObservable().Subscribe(
        [&](int) { on_next_called = true; },
        [&](std::exception_ptr) { on_error_called = true; },
        [&]() { on_completed_called = true; }
    );
    ASSERT_TRUE(sub->IsDisposed());
    ASSERT_FALSE(on_next_called);
    ASSERT_FALSE(on_error_called);
    ASSERT_TRUE(on_completed_called);
}


TEST(RxSubjectTest, OnError) {

    zaf::rx::Subject<int> subject;

    std::vector<int> values;
    int on_error_count{};
    int on_completed_count{};
    auto sub = subject.AsObservable().Subscribe([&values](int value) {
        values.push_back(value);
    }, 
    [&](std::exception_ptr) {
        on_error_count++;
    }, 
    [&]() {
        on_completed_count++;
    });

    subject.AsObserver().OnNext(8);
    subject.AsObserver().OnNext(5);
    subject.AsObserver().OnNext(3);
    subject.AsObserver().OnError(std::make_exception_ptr(std::runtime_error("error")));

    std::vector<int> expected{ 8, 5, 3 };
    ASSERT_EQ(values, expected);
    ASSERT_EQ(on_error_count, 1);
    ASSERT_EQ(on_completed_count, 0);

    // Any emission will be ignored after termination.
    subject.AsObserver().OnNext(4);
    subject.AsObserver().OnError(std::make_exception_ptr(std::runtime_error("")));
    subject.AsObserver().OnCompleted();
    ASSERT_EQ(values, expected);
    ASSERT_EQ(on_error_count, 1);
    ASSERT_EQ(on_completed_count, 0);
}


/**
This test case emits OnError multiple times, to make sure the second emission will be ignored and
the error information won't be overwritten.
*/
TEST(RxSubjectTest, MultipleOnError) {

    zaf::rx::Subject<int> subject;
    subject.AsObserver().OnError(std::make_exception_ptr(std::runtime_error("error")));
    subject.AsObserver().OnError(std::make_exception_ptr(std::runtime_error("error2")));

    std::optional<std::runtime_error> error;
    bool on_next_called{};
    bool on_completed_called{};
    auto sub = subject.AsObservable().Subscribe([&](int value) {
        on_next_called = true;
    },
    [&](std::exception_ptr exception) {
        try {
            std::rethrow_exception(exception);
        }
        catch (const std::runtime_error& e) {
            error = e;
        }
    },
    [&]() {
        on_completed_called = true;
    });
    ASSERT_TRUE(sub->IsDisposed());
    ASSERT_TRUE(error.has_value());
    ASSERT_STREQ(error->what(), "error");
    ASSERT_FALSE(on_next_called);
    ASSERT_FALSE(on_completed_called);
}


TEST(RxSubjectTest, SubscribeAfterOnError) {

    zaf::rx::Subject<int> subject;
    subject.AsObserver().OnNext(18);
    subject.AsObserver().OnError(std::make_exception_ptr(std::runtime_error("error")));

    bool on_next_called{};
    bool on_error_called{};
    bool on_completed_called{};

    // Subscribing to a terminated subject will receive OnError immediately.
    auto sub = subject.AsObservable().Subscribe(
        [&](int) { on_next_called = true; },
        [&](std::exception_ptr) { on_error_called = true; },
        [&]() { on_completed_called = true; }
    );
    ASSERT_TRUE(sub->IsDisposed());
    ASSERT_FALSE(on_next_called);
    ASSERT_TRUE(on_error_called);
    ASSERT_FALSE(on_completed_called);
}


/*
This test case destroys a terminated subject in the OnCompleted notification, to make sure it won't 
crash in such case.
*/
TEST(RxSubjectTest, DestroySubjectInNewSubscriptionAfterTermination) {

    std::optional<zaf::rx::Subject<int>> subject;
    subject.emplace();

    subject->AsObserver().OnCompleted();

    bool on_completed_called{};
    auto sub = subject->AsObservable().Subscribe(
        [](int) {},
        [&]() {
        on_completed_called = true;
        subject.reset();
    }
    );
    ASSERT_TRUE(on_completed_called);
}


/**
This test case destroy the subject which there are still subscriptions on it, to make sure the
subscriptions will be unsubscribed properly.
*/
TEST(RxSubjectTest, UnsubscribeOnDestroy) {

    std::optional<zaf::rx::Subject<int>> subject;
    subject.emplace();

    auto sub1 = subject->AsObservable().Subscribe();
    auto sub2 = subject->AsObservable().Subscribe();

    subject.reset();
    ASSERT_TRUE(sub1->IsDisposed());
    ASSERT_TRUE(sub2->IsDisposed());
}


TEST(RxSubjectTest, CancelSubscriptionExplicit) {

    zaf::rx::Subject<int> subject;

    std::vector<int> values;
    auto subscription = subject.AsObservable().Subscribe(
        [&values](int value) {

        values.push_back(value);
    });

    subject.AsObserver().OnNext(1);
    subscription->Dispose();
    subject.AsObserver().OnNext(2);

    std::vector<int> expected{ 1 };
    ASSERT_EQ(values, expected);
}


TEST(RxSubjectTest, CancelSubscriptionImplicit) {

    zaf::rx::Subject<int> subject;

    std::vector<int> values;
    {
        auto subscription = subject.AsObservable().Subscribe(
            [&values](int value) {

            values.push_back(value);
        });

        subject.AsObserver().OnNext(1);
    }
    subject.AsObserver().OnNext(2);

    std::vector<int> expected{ 1 };
    ASSERT_EQ(values, expected);
}


TEST(RxSubjectTest, SubscriptionCount) {

    zaf::rx::Subject<int> subject;
    ASSERT_EQ(subject.Core()->SubscriptionCount(), 0);

    auto subscription1 = subject.AsObservable().Subscribe([](int) {});
    ASSERT_EQ(subject.Core()->SubscriptionCount(), 1);

    auto subscription2 = subject.AsObservable().Subscribe([](int) {});
    ASSERT_EQ(subject.Core()->SubscriptionCount(), 2);

    subscription1->Dispose();
    ASSERT_EQ(subject.Core()->SubscriptionCount(), 1);

    subscription2->Dispose();
    ASSERT_EQ(subject.Core()->SubscriptionCount(), 0);
}