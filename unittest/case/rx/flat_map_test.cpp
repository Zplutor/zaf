#include <gtest/gtest.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/subject.h>

TEST(RxFlatMapTest, FlatMap) {

    zaf::Subject<int> subject;

    std::vector<std::string> result;
    bool on_completed_called{};
    auto sub = subject.AsObservable().FlatMap<std::string>([](int value) {
        return zaf::rx::Just(std::to_string(value));
    })
    .Subscribe([&](const std::string& value) {
        result.push_back(value);
    },
    [&]() {
        on_completed_called = true;
    });

    subject.AsObserver().OnNext(3);
    subject.AsObserver().OnNext(33);
    subject.AsObserver().OnNext(333);
    subject.AsObserver().OnCompleted();

    std::vector<std::string> expected{ "3", "33", "333" };
    ASSERT_EQ(result, expected);
    ASSERT_TRUE(on_completed_called);
}