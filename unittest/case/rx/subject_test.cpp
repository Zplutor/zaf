#include <gtest/gtest.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/internal/subject/inner_subject.h>

static_assert(!std::is_copy_assignable_v<zaf::Subject<int>>);
static_assert(!std::is_copy_constructible_v<zaf::Subject<int>>);
static_assert(std::is_move_assignable_v<zaf::Subject<int>>);
static_assert(std::is_move_constructible_v<zaf::Subject<int>>);


TEST(RxSubjectTest, Normal) {

    zaf::Subject<int> subject;

    std::vector<int> values;
    auto subscription = subject.AsObservable().Subscribe(
        [&values](int value) {
    
        values.push_back(value);
    });

    subject.AsObserver().OnNext(8);
    subject.AsObserver().OnNext(5);
    subject.AsObserver().OnNext(0);
    subject.AsObserver().OnNext(3);

    std::vector<int> expected{ 8, 5, 0, 3 };
    ASSERT_EQ(values, expected);
}


TEST(RxSubjectTest, CancelSubscriptionExplicit) {

    zaf::Subject<int> subject;

    std::vector<int> values;
    auto subscription = subject.AsObservable().Subscribe(
        [&values](int value) {

        values.push_back(value);
    });

    subject.AsObserver().OnNext(1);
    subscription.Unsubscribe();
    subject.AsObserver().OnNext(2);

    std::vector<int> expected{ 1 };
    ASSERT_EQ(values, expected);
}


TEST(RxSubjectTest, CancelSubscriptionImplicit) {

    zaf::Subject<int> subject;

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

    zaf::Subject<int> subject;
    ASSERT_EQ(subject.Inner()->SubscriptionCount(), 0);

    auto subscription1 = subject.AsObservable().Subscribe([](int) {});
    ASSERT_EQ(subject.Inner()->SubscriptionCount(), 1);

    auto subscription2 = subject.AsObservable().Subscribe([](int) {});
    ASSERT_EQ(subject.Inner()->SubscriptionCount(), 2);

    subscription1.Unsubscribe();
    ASSERT_EQ(subject.Inner()->SubscriptionCount(), 1);

    subscription2.Unsubscribe();
    ASSERT_EQ(subject.Inner()->SubscriptionCount(), 0);
}