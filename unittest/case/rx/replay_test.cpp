#include <gtest/gtest.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subjects/subject.h>

TEST(RxReplayTest, Replay_NoReplaySize) {

    zaf::rx::Subject<int> subject;
    zaf::rx::ConnectableObservable observable = subject.AsObservable().Replay();

    std::vector<int> on_next_values;
    auto sub1 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 1);
    });
    auto sub2 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 2);
    });
    subject.AsObserver().OnNext(4);
    ASSERT_TRUE(on_next_values.empty());

    auto connect_sub = observable.Connect();
    ASSERT_TRUE(on_next_values.empty());

    subject.AsObserver().OnNext(6);
    subject.AsObserver().OnNext(7);
    ASSERT_EQ(on_next_values, (std::vector<int>{ 7, 8, 8, 9 }));

    auto sub3 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 3);
    });
    ASSERT_EQ(on_next_values, (std::vector<int>{ 7, 8, 8, 9, 9, 10 }));
}


TEST(RxReplayTest, Replay_ReplaySize) {

    zaf::rx::Subject<int> subject;
    zaf::rx::ConnectableObservable observable = subject.AsObservable().Replay(1);

    std::vector<int> on_next_values;
    auto sub1 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 1);
    });
    auto sub2 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 2);
    });
    subject.AsObserver().OnNext(4);
    ASSERT_TRUE(on_next_values.empty());

    auto connect_sub = observable.Connect();
    ASSERT_TRUE(on_next_values.empty());

    subject.AsObserver().OnNext(6);
    subject.AsObserver().OnNext(7);
    ASSERT_EQ(on_next_values, (std::vector<int>{ 7, 8, 8, 9 }));

    auto sub3 = observable.Subscribe([&](int value) {
        on_next_values.push_back(value + 3);
    });
    ASSERT_EQ(on_next_values, (std::vector<int>{ 7, 8, 8, 9, 10 }));
}