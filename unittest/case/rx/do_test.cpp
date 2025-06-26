#include <gtest/gtest.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

TEST(RxDoTest, OnNext) {

    std::vector<int> do_values;
    std::vector<int> observed_values;

    zaf::Subject<int> subject;
    auto subscription = subject.AsObservable().Do([&do_values](int value) {
        do_values.push_back(value);
    })
    .Subscribe([&observed_values](int value) {
        observed_values.push_back(value);
    });

    subject.AsObserver().OnNext(78);
    subject.AsObserver().OnNext(2487);
    subject.AsObserver().OnNext(-23);

    std::vector<int> expected{ 78, 2487, -23 };
    ASSERT_EQ(expected, do_values);
    ASSERT_EQ(expected, observed_values);
}


TEST(RxDoTest, OnNextOnCompleted) {

    int call_sequence{};

    int do_value{};
    int do_value_sequence{};
    int do_completed_sequence{};

    int observed_value{};
    int observed_value_sequence{};
    int observed_completed_sequence{};

    zaf::Subject<int> subject;
    auto subscription = subject.AsObservable().Do([&](int value) {
        do_value = value;
        do_value_sequence = ++call_sequence;
    }, 
    [&]() {
        do_completed_sequence = ++call_sequence;
    })
    .Subscribe([&](int value) {
        observed_value = value;
        observed_value_sequence = ++call_sequence;
    },
    [&]() {
        observed_completed_sequence = ++call_sequence;
    });

    subject.AsObserver().OnNext(100);
    subject.AsObserver().OnCompleted();

    ASSERT_EQ(do_value, 100);
    ASSERT_EQ(observed_value, 100);

    ASSERT_EQ(do_value_sequence, 1);
    ASSERT_EQ(observed_value_sequence, 2);
    ASSERT_EQ(do_completed_sequence, 3);
    ASSERT_EQ(observed_completed_sequence, 4);
}


TEST(RxDoTest, OnNextOnError) {

    int call_sequence{};

    int do_value{};
    int do_value_sequence{};
    int do_error_sequence{};

    int observed_value{};
    int observed_value_sequence{};
    int observed_error_sequence{};

    zaf::Subject<int> subject;
    auto subscription = subject.AsObservable().Do(
        [&](int value) {
            do_value = value;
            do_value_sequence = ++call_sequence;
        },
        [&](const std::exception_ptr& error) {
            do_error_sequence = ++call_sequence;
        })
    .Subscribe(
        [&](int value) {
            observed_value = value;
            observed_value_sequence = ++call_sequence;
        }, 
        [&](const std::exception_ptr& error) {
            observed_error_sequence = ++call_sequence;
        });

    subject.AsObserver().OnNext(99);
    subject.AsObserver().OnError(std::make_exception_ptr(1));

    ASSERT_EQ(do_value, 99);
    ASSERT_EQ(observed_value, 99);

    ASSERT_EQ(do_value_sequence, 1);
    ASSERT_EQ(observed_value_sequence, 2);
    ASSERT_EQ(do_error_sequence, 3);
    ASSERT_EQ(observed_error_sequence, 4);
}


TEST(RxDoTest, OnNextOnErrorOnCompleted) {

    struct TestState {
        int do_value{};
        bool do_error{};
        bool do_completed{};

        int observed_value{};
        bool observed_error{};
        bool observed_completed{};
    };

    TestState test_state;
    zaf::Subscription subscription;
    auto create_test_subject = [&]() {
    
        test_state = {};

        zaf::Subject<int> subject;
        subscription = subject.AsObservable().Do(
            [&](int value) {
                test_state.do_value = value;
            },
            [&](const std::exception_ptr& error) {
                test_state.do_error = true;
            },
            [&]() {
                test_state.do_completed = true;
            }
        )
        .Subscribe(
            [&](int value) {
                test_state.observed_value = value;
            },
            [&](const std::exception_ptr& error) {
                test_state.observed_error = true;
            },
            [&]() {
                test_state.observed_completed = true;
            }
        );

        return subject;
    };

    {
        auto subject = create_test_subject();
        subject.AsObserver().OnNext(11);
        subject.AsObserver().OnError(std::make_exception_ptr(2));

        ASSERT_EQ(test_state.do_value, 11);
        ASSERT_TRUE(test_state.do_error);
        ASSERT_FALSE(test_state.do_completed);

        ASSERT_EQ(test_state.observed_value, 11);
        ASSERT_TRUE(test_state.observed_error);
        ASSERT_FALSE(test_state.observed_completed);
    }

    {
        auto subject = create_test_subject();
        subject.AsObserver().OnNext(12);
        subject.AsObserver().OnCompleted();

        ASSERT_EQ(test_state.do_value, 12);
        ASSERT_FALSE(test_state.do_error);
        ASSERT_TRUE(test_state.do_completed);

        ASSERT_EQ(test_state.observed_value, 12);
        ASSERT_FALSE(test_state.observed_error);
        ASSERT_TRUE(test_state.observed_completed);
    }
}


TEST(RxDoTest, Unsubscribe) {

    std::vector<int> do_values;
    std::vector<int> observed_values;

    zaf::Subject<int> subject;
    auto subscription = subject.AsObservable().Do([&do_values](int value) {
        do_values.push_back(value);
    })
    .Subscribe([&observed_values](int value) {
        observed_values.push_back(value);
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subscription.Unsubscribe();
    subject.AsObserver().OnNext(3);

    std::vector<int> expected{ 1, 2 };
    ASSERT_EQ(expected, do_values);
    ASSERT_EQ(expected, observed_values);
}


TEST(RxDoTest, CircularReference) {

    zaf::Subject<int> subject;
    auto observable = subject.AsObservable().Do([](int) {});
    auto subscription = observable.Subscribe();

    std::weak_ptr<zaf::rx::internal::Producer> weak_ptr = subscription.Inner()->Producer();
    subscription = {};

    auto shared_ptr = weak_ptr.lock();
    ASSERT_EQ(shared_ptr, nullptr);
}


TEST(RxDoTest, SubscribeMultipleTimes) {

    int call_times{};

    zaf::Subject<int> subject;
    auto observable = subject.AsObservable().Do([&](int) {
        ++call_times;
    });

    auto sub1 = observable.Subscribe();
    auto sub2 = observable.Subscribe();

    subject.AsObserver().OnNext(1);
    ASSERT_EQ(call_times, 2);
}