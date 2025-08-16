#include <gtest/gtest.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/dispose_bag.h>

static_assert(!std::is_copy_assignable_v<zaf::rx::DisposeBag>);
static_assert(!std::is_copy_constructible_v<zaf::rx::DisposeBag>);
static_assert(!std::is_move_assignable_v<zaf::rx::DisposeBag>);
static_assert(!std::is_move_constructible_v<zaf::rx::DisposeBag>);


TEST(RxSubscriptionBagTest, EmptySubscription) {

    zaf::rx::DisposeBag bag;
    bag += nullptr;
    ASSERT_EQ(bag.Count(), 0);

    std::vector<float> values;
    bag += zaf::rx::Observable<float>::Just(0.5f).Subscribe([&values](float value) {
        values.push_back(value);
    });

    std::vector<float> expected{ 0.5f };
    ASSERT_EQ(values, expected);
    ASSERT_EQ(bag.Count(), 0);
}


TEST(RxSubscriptionBagTest, UnsubscribedSubscription) {

    zaf::rx::Subject<std::string> subject;
    auto sub = subject.AsObservable().Subscribe();
    sub->Dispose();

    zaf::rx::DisposeBag bag;
    bag += sub;
    ASSERT_EQ(bag.Count(), 0);
}


TEST(RxSubscriptionBagTest, CancelSubscription) {

    zaf::rx::Subject<std::string> subject;
    zaf::rx::DisposeBag bag;

    std::vector<std::string> values;
    bag += subject.AsObservable().Subscribe([&values](const std::string& value) {
        values.push_back(value);
    });
    ASSERT_EQ(bag.Count(), 1);

    subject.AsObserver().OnNext("before");
    bag.Clear();
    ASSERT_EQ(bag.Count(), 0);
    subject.AsObserver().OnNext("after");

    std::vector<std::string> expected{ "before" };
    ASSERT_EQ(values, expected);
}


TEST(RxSubscriptionBagTest, CancelSubscriptionByDestruction) {

    zaf::rx::Subject<std::string> subject;
    auto bag = std::make_unique<zaf::rx::DisposeBag>();

    std::vector<std::string> values;
    *bag += subject.AsObservable().Subscribe([&values](const std::string& value) {
        values.push_back(value);
    });
    ASSERT_EQ(bag->Count(), 1);

    subject.AsObserver().OnNext("before");
    bag.reset();
    subject.AsObserver().OnNext("after");

    std::vector<std::string> expected{ "before" };
    ASSERT_EQ(values, expected);
}


TEST(RxSubscriptionBagTest, RemoveAfterFinish) {

    zaf::rx::DisposeBag bag;

    //Remove after OnError
    {
        zaf::rx::Subject<std::string> subject;
        bag += subject.AsObservable().Subscribe([](std::string) {});
        subject.AsObserver().OnNext("a");
        ASSERT_EQ(bag.Count(), 1);
        subject.AsObserver().OnError(std::make_exception_ptr(10));
        ASSERT_EQ(bag.Count(), 0);
    }

    //Remove after OnCompleted
    {
        zaf::rx::Subject<std::string> subject;
        bag += subject.AsObservable().Subscribe([](std::string) {});
        subject.AsObserver().OnNext("b");
        ASSERT_EQ(bag.Count(), 1);
        subject.AsObserver().OnCompleted();
        ASSERT_EQ(bag.Count(), 0);
    }

    //Remove after Dispose
    {
        zaf::rx::Subject<std::string> subject;
        auto subscription = subject.AsObservable().Subscribe([](std::string) {});
        bag += subscription;
        subject.AsObserver().OnNext("c");
        ASSERT_EQ(bag.Count(), 1);
        subscription->Dispose();
        ASSERT_EQ(bag.Count(), 0);
    }
}