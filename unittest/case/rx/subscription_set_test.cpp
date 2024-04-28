#include <gtest/gtest.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_set.h>

static_assert(!std::is_copy_assignable_v<zaf::SubscriptionSet>);
static_assert(!std::is_copy_constructible_v<zaf::SubscriptionSet>);
static_assert(!std::is_move_assignable_v<zaf::SubscriptionSet>);
static_assert(!std::is_move_constructible_v<zaf::SubscriptionSet>);


TEST(RxSubscriptionSetTest, EmptySubscription) {

    zaf::SubscriptionSet set;
    set += zaf::Subscription{};
    ASSERT_EQ(set.Count(), 0);

    set += "empty" / zaf::Subscription{};
    ASSERT_EQ(set.Count(), 0);

    std::vector<float> values;
    set += zaf::rx::Just<float>(0.5f).Subscribe([&values](float value) {
        values.push_back(value);
    });

    std::vector<float> expected{ 0.5f };
    ASSERT_EQ(values, expected);
    ASSERT_EQ(set.Count(), 0);
}


TEST(RxSubscriptionSetTest, CancelSubscription) {

    zaf::Subject<std::string> subject;
    zaf::SubscriptionSet set;

    std::vector<std::string> values;
    set += subject.AsObservable().Subscribe([&values](const std::string& value) {
        values.push_back(value);
    });
    ASSERT_EQ(set.Count(), 1);

    subject.AsObserver().OnNext("before");
    set.Clear();
    ASSERT_EQ(set.Count(), 0);
    subject.AsObserver().OnNext("after");

    std::vector<std::string> expected{ "before" };
    ASSERT_EQ(values, expected);
}


TEST(RxSubscriptionSetTest, CancelSubscriptionWithTag) {

    zaf::Subject<std::string> subject;
    zaf::SubscriptionSet set;

    std::vector<std::string> values;
    set += "tag" / subject.AsObservable().Subscribe([&values](const std::string& value) {
        values.push_back(value);
    });
    ASSERT_EQ(set.Count(), 1);

    subject.AsObserver().OnNext("before");
    set -= "tag";
    ASSERT_EQ(set.Count(), 0);
    subject.AsObserver().OnNext("after");

    std::vector<std::string> expected{ "before" };
    ASSERT_EQ(values, expected);
}


TEST(RxSubscriptionSetTest, CancelSubscriptionByDestruction) {

    zaf::Subject<std::string> subject;
    auto set = std::make_unique<zaf::SubscriptionSet>();

    std::vector<std::string> values;
    *set += subject.AsObservable().Subscribe([&values](const std::string& value) {
        values.push_back(value);
    });
    *set += "tag" / subject.AsObservable().Subscribe([&values](const std::string& value) {
        values.push_back(value);
    });
    ASSERT_EQ(set->Count(), 2);

    subject.AsObserver().OnNext("before");
    set.reset();
    subject.AsObserver().OnNext("after");

    std::vector<std::string> expected{ "before", "before"};
    ASSERT_EQ(values, expected);
}


TEST(RxSubscriptionSetTest, RemoveAfterFinish) {

    zaf::SubscriptionSet set;
    zaf::Subject<std::string> subject;

    //Remove after OnError
    set += subject.AsObservable().Subscribe([](std::string) {});
    subject.AsObserver().OnNext("a");
    ASSERT_EQ(set.Count(), 1);
    subject.AsObserver().OnError(std::make_exception_ptr(10));
    ASSERT_EQ(set.Count(), 0);

    //Remove after OnCompleted
    set += subject.AsObservable().Subscribe([](std::string) {});
    subject.AsObserver().OnNext("b");
    ASSERT_EQ(set.Count(), 1);
    subject.AsObserver().OnCompleted();
    ASSERT_EQ(set.Count(), 0);

    //Remove after Unsuscribe
    auto subscription = subject.AsObservable().Subscribe([](std::string) {});
    set += subscription;
    subject.AsObserver().OnNext("c");
    ASSERT_EQ(set.Count(), 1);
    subscription.Unsubscribe();
    ASSERT_EQ(set.Count(), 0);
}