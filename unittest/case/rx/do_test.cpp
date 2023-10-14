#include <gtest/gtest.h>
#include <zaf/rx/subject.h>

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

    int do_value{};
    bool do_completed{};

    int observed_value{};
    bool observed_completed{};

    zaf::Subject<int> subject;
    auto subscription = subject.AsObservable().Do([&](int value) {
        do_value = value;
    }, 
    [&]() {
        do_completed = true;
    })
    .Subscribe([&](int value) {
        observed_value = value;
    },
    [&]() {
        observed_completed = true;
    });

    subject.AsObserver().OnNext(100);
    subject.AsObserver().OnCompleted();

    ASSERT_EQ(do_value, 100);
    ASSERT_TRUE(do_completed);
    ASSERT_EQ(observed_value, 100);
    ASSERT_TRUE(observed_completed);
}


TEST(RxDoTest, OnNextOnError) {

    int do_value{};
    bool do_error{};

    int observed_value{};
    bool observed_error{};

    zaf::Subject<int> subject;
    auto subscription = subject.AsObservable().Do([&](int value) {
        do_value = value;
    },
    [&](const zaf::Error& error) {
        do_error = true;
    })
    .Subscribe([&](int value) {
        observed_value = value;
    }, 
    [&](const zaf::Error& error) {
        observed_error = true;
    });

    subject.AsObserver().OnNext(99);
    subject.AsObserver().OnError(zaf::Error(std::make_error_code(std::errc::bad_message)));

    ASSERT_EQ(do_value, 99);
    ASSERT_TRUE(do_error);
    ASSERT_EQ(observed_value, 99);
    ASSERT_TRUE(observed_error);
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
            [&](const zaf::Error& error) {
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
            [&](const zaf::Error& error) {
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
        subject.AsObserver().OnError(zaf::Error(std::make_error_code(std::errc::bad_message)));

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