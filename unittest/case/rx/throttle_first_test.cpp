#include <thread>
#include <gtest/gtest.h>
#include <zaf/rx/subject/subject.h>

TEST(RxThrottleFirstTest, Throttle) {

    zaf::rx::Subject<int> subject;
    std::vector<int> values;
    auto sub = subject.AsObservable().ThrottleFirst(std::chrono::milliseconds(50)).Subscribe(
        [&values](int value) {
            values.push_back(value);
        });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    subject.AsObserver().OnNext(3);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    subject.AsObserver().OnNext(4);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    subject.AsObserver().OnNext(5);

    ASSERT_EQ(values, (std::vector<int>{ 1, 4, 5 }));
}