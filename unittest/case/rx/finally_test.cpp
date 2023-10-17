#include <gtest/gtest.h>
#include <zaf/rx/subject.h>

TEST(RxFinallyTest, Finally) {

    struct TestState {
        bool finally_called{};
        bool on_error_called{};
        bool on_completed_called{};
        int on_next_value{};
    };

    TestState test_state;
    zaf::Subscription subscription;
    auto create_test_subject = [&]() {

        zaf::Subject<int> subject;
        subscription = subject.AsObservable().Finally([&]() {
            test_state.finally_called = true;
        })
        .Subscribe([&](int value) {
            test_state.on_next_value = value;
        },
        [&](const zaf::Error&) {
            test_state.on_error_called = true;
        },
        [&]() {
            test_state.on_completed_called = true;
        });
        return subject;
    };

    //Call on unsubscribe
    {
        test_state = {};
        auto subject = create_test_subject();

        subject.AsObserver().OnNext(1);
        ASSERT_EQ(test_state.on_next_value, 1);
        ASSERT_EQ(test_state.on_error_called, false);
        ASSERT_EQ(test_state.on_completed_called, false);
        ASSERT_EQ(test_state.finally_called, false);
        
        subject.AsObserver().OnNext(2);
        ASSERT_EQ(test_state.on_next_value, 2);
        ASSERT_EQ(test_state.on_error_called, false);
        ASSERT_EQ(test_state.on_completed_called, false);
        ASSERT_EQ(test_state.finally_called, false);

        subscription.Unsubscribe();
        ASSERT_EQ(test_state.on_next_value, 2);
        ASSERT_EQ(test_state.on_error_called, false);
        ASSERT_EQ(test_state.on_completed_called, false);
        ASSERT_EQ(test_state.finally_called, true);
    }

    //Call on OnError
    {
        test_state = {};
        auto subject = create_test_subject();

        subject.AsObserver().OnNext(3);
        subject.AsObserver().OnError(zaf::Error(std::make_error_code(std::errc::bad_address)));
        ASSERT_EQ(test_state.on_next_value, 3);
        ASSERT_EQ(test_state.on_error_called, true);
        ASSERT_EQ(test_state.on_completed_called, false);
        ASSERT_EQ(test_state.finally_called, true);

        test_state = {};
        subscription.Unsubscribe();
        ASSERT_EQ(test_state.finally_called, false);
    }

    //Call on OnCompleted
    {
        test_state = {};
        auto subject = create_test_subject();

        subject.AsObserver().OnNext(4);
        subject.AsObserver().OnCompleted();
        ASSERT_EQ(test_state.on_next_value, 4);
        ASSERT_EQ(test_state.on_error_called, false);
        ASSERT_EQ(test_state.on_completed_called, true);
        ASSERT_EQ(test_state.finally_called, true);

        test_state = {};
        subscription.Unsubscribe();
        ASSERT_EQ(test_state.finally_called, false);
    }
}


TEST(RxFinallyTest, MultipleFinally) {

    int seed{};
    int finally1_value{};
    int finally2_value{};

    zaf::Subject<int> subject;
    auto subscription = subject.AsObservable().Finally([&]() {
        finally1_value = ++seed;
    })
    .Finally([&]() {
        finally2_value = ++seed;
    })
    .Subscribe([](int) {});

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnCompleted();

    ASSERT_EQ(finally1_value, 1);
    ASSERT_EQ(finally2_value, 2);
}