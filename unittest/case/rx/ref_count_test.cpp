#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/internal/operator/ref_count_operator.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject/subject.h>

TEST(RxRefCountTest, Publish) {

    zaf::rx::Subject<int> subject;

    auto connectable_observable = subject.AsObservable().Publish();
    auto ref_count_observable = connectable_observable.RefCount();
    auto ref_count_operator = 
        zaf::As<zaf::rx::internal::RefCountOperator>(ref_count_observable.Core());

    // The connectable observable will be connected when the first subscription is made.
    std::vector<int> values;
    auto sub1 = ref_count_observable.Subscribe([&](int value) {
        values.push_back(value);
    });
    ASSERT_TRUE(ref_count_operator->HasConnection());

    subject.AsObserver().OnNext(1);
    ASSERT_EQ(values, std::vector<int>{ 1 });

    auto sub2 = ref_count_observable.Subscribe([&](int value) {
        values.push_back(value);
    });

    subject.AsObserver().OnNext(2);
    ASSERT_EQ(values, (std::vector<int>{ 1, 2, 2 }));

    sub1->Dispose();
    subject.AsObserver().OnNext(3);
    ASSERT_EQ(values, (std::vector<int>{ 1, 2, 2, 3 }));

    // The connection will be disconnected when the last subscription is unsubscribed.
    sub2->Dispose();
    subject.AsObserver().OnNext(4);
    ASSERT_EQ(values, (std::vector<int>{ 1, 2, 2, 3 }));
    ASSERT_FALSE(ref_count_operator->HasConnection());
}


/*
In this test case, the underlying observable is terminated immediately after it is connected.
*/
TEST(RxRefCountTest, TerminateOnConnect) {

    auto observable = zaf::rx::Observable<int>::Just(12).Publish().RefCount();
    auto ref_count_operator =
        zaf::As<zaf::rx::internal::RefCountOperator>(observable.Core());

    // The first subscription.
    {
        std::vector<int> values;
        int on_error_count{};
        int on_completed_count{};
        auto sub1 = observable.Subscribe([&](int value) {
            values.push_back(value);
        }, 
        [&](std::exception_ptr) {
            on_error_count++;
        },
        [&]() {
            on_completed_count++;
        });
        ASSERT_EQ(values, std::vector<int>{ 12 });
        ASSERT_EQ(on_error_count, 0);
        ASSERT_EQ(on_completed_count, 1);
        ASSERT_FALSE(ref_count_operator->HasConnection());
    }

    {
        // The second subscription. It won't receive any value because the published observable
        // is already terminated.
        int on_next_count{};
        int on_error_count{};
        int on_completed_count{};
        auto sub2 = observable.Subscribe([&](int value) {
            on_next_count++;
        },
        [&](std::exception_ptr) {
            on_error_count++;
        },
        [&]() {
            on_completed_count++;
        });
        ASSERT_EQ(on_next_count, 0);
        ASSERT_EQ(on_error_count, 0);
        ASSERT_EQ(on_completed_count, 1);
        ASSERT_FALSE(ref_count_operator->HasConnection());
    }
}


/*
In this test case, the subscription is unsubscribed immediately after it is subscribed.
*/
TEST(RxRefCountTest, TerminateOnSubscribe) {

    zaf::rx::Subject<int> subject;

    auto observable = subject.AsObservable().Replay().RefCount();
    auto ref_count_operator =
        zaf::As<zaf::rx::internal::RefCountOperator>(observable.Core());

    std::vector<int> values;
    auto sub1 = observable.Subscribe([&values](int value) {
        values.push_back(value);
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnCompleted();
    ASSERT_EQ(values, std::vector<int>{ 1 });
    ASSERT_FALSE(ref_count_operator->HasConnection());

    // This subscription will be unsubscribed immediately after it is subscribed.
    auto sub2 = observable.Subscribe([&values](int value) {
        values.push_back(value);
    });
    ASSERT_EQ(values, (std::vector<int>{ 1, 1 }));
    ASSERT_FALSE(ref_count_operator->HasConnection());
}