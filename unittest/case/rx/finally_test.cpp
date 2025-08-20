#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/single.h>
#include <zaf/rx/subject/subject.h>

TEST(RxFinallyTest, Finally) {

    struct TestState {
        bool finally_called{};
        bool on_error_called{};
        bool on_completed_called{};
        int on_next_value{};
    };

    TestState test_state;
    std::shared_ptr<zaf::rx::Disposable> subscription;
    auto create_test_subject = [&]() {

        zaf::rx::Subject<int> subject;
        subscription = subject.AsObservable().Finally([&]() {
            test_state.finally_called = true;
        })
        .Subscribe([&](int value) {
            test_state.on_next_value = value;
        },
        [&](const std::exception_ptr&) {
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

        subscription->Dispose();
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
        subject.AsObserver().OnError(std::make_exception_ptr(3));
        ASSERT_EQ(test_state.on_next_value, 3);
        ASSERT_EQ(test_state.on_error_called, true);
        ASSERT_EQ(test_state.on_completed_called, false);
        ASSERT_EQ(test_state.finally_called, true);

        test_state = {};
        subscription->Dispose();
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
        subscription->Dispose();
        ASSERT_EQ(test_state.finally_called, false);
    }
}


TEST(RxFinallyTest, MultipleFinallyOrder) {

    int seed{};
    int finally1_value{};
    int finally2_value{};

    zaf::rx::Subject<int> subject;
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


TEST(RxFinallyTest, SubscribeMultipleTimes) {

    int call_times{};

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Finally([&]() {
        ++call_times;
    });

    auto sub1 = observable.Subscribe();
    auto sub2 = observable.Subscribe();

    subject.AsObserver().OnCompleted();
    ASSERT_EQ(call_times, 2);
}


// The application will terminate if an exception is thrown in Finally.
TEST(RxFinallyTest, ThrowException) {

    auto observable = zaf::rx::Observable<int>::Just(27).Finally([]() {
        throw zaf::InvalidOperationError{};
    });
    ASSERT_DEATH(auto sub = observable.Subscribe(), ".*");
}


TEST(RxFinallyTest, Single) {

    // Finally with OnSuccess
    {
        auto single = zaf::rx::Single<int>::Just(34);
        bool finally_called = false;
        auto sub = single.Finally([&]() {
            finally_called = true;
        }).Subscribe();
        ASSERT_TRUE(finally_called);
    }

    // Finally with OnError
    {
        auto single = zaf::rx::Single<int>::Throw(zaf::InvalidOperationError{ "error" });
        bool finally_called = false;
        auto sub = single.Finally([&]() {
            finally_called = true;
        }).Subscribe();
        ASSERT_TRUE(finally_called);
    }

    // Finally on unsubscribe
    {
        auto single = zaf::rx::Single<int>::Never();
        bool finally_called = false;
        {
            auto sub = single.Finally([&]() {
                finally_called = true;
            }).Subscribe();
        }
        ASSERT_TRUE(finally_called);
    }
}