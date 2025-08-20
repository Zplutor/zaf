#include <gtest/gtest.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>

TEST(RxPublishTest, ColdObservable) {

    zaf::rx::ConnectableObservable<int> observable = zaf::rx::Observable<int>::Just(52).Publish();

    // Subscribe to a published observable won't trigger any emission.
    std::vector<int> on_next_values;
    auto sub1 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 1);
    });
    ASSERT_TRUE(on_next_values.empty());

    auto sub2 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 2);
    });
    ASSERT_TRUE(on_next_values.empty());

    // Connect the observable to start emission.
    auto connect_sub = observable.Connect();
    ASSERT_EQ(on_next_values, (std::vector<int>{ 53, 54 }));
}


TEST(RxPublishTest, HotObservable) {

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Publish();

    std::vector<int> on_next_values;
    auto sub1 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 1);
    });
    auto sub2 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 2);
    });
    subject.AsObserver().OnNext(12);
    ASSERT_TRUE(on_next_values.empty());

    auto connect_sub = observable.Connect();
    ASSERT_TRUE(on_next_values.empty());

    subject.AsObserver().OnNext(13);
    ASSERT_EQ(on_next_values, (std::vector<int>{ 14, 15 }));

    auto sub3 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 3);
    });
    ASSERT_EQ(on_next_values, (std::vector<int>{ 14, 15 }));

    subject.AsObserver().OnNext(13);
    ASSERT_EQ(on_next_values, (std::vector<int>{ 14, 15, 14, 15, 16 }));
}


TEST(RxPublishTest, Dispose) {

    zaf::rx::Subject<int> subject;
    auto observable = subject.AsObservable().Publish();

    std::vector<int> on_next_values;
    auto sub1 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 1);
    });
    auto sub2 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 2);
    });

    auto connect_sub = observable.Connect();
    subject.AsObserver().OnNext(16);

    connect_sub->Dispose();
    subject.AsObserver().OnNext(16);
    ASSERT_EQ(on_next_values, (std::vector<int>{ 17, 18 }));
}