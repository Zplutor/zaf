#include <gtest/gtest.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/subject.h>

TEST(RxFlatMapTest, FlatMap) {

    zaf::Subject<int> subject;

    std::vector<std::string> result;
    int completed_count{};
    auto sub = subject.AsObservable().FlatMap<std::string>([](int value) {
        return zaf::rx::Just(std::to_string(value));
    })
    .Subscribe([&](const std::string& value) {
        result.push_back(value);
    },
    [&]() {
        completed_count++;
    });

    subject.AsObserver().OnNext(3);
    subject.AsObserver().OnNext(33);
    subject.AsObserver().OnNext(333);
    subject.AsObserver().OnCompleted();

    std::vector<std::string> expected{ "3", "33", "333" };
    ASSERT_EQ(result, expected);
    ASSERT_EQ(completed_count, 1);
}


TEST(FlatMapTest, ErrorFromSource) {

    zaf::Subject<int> subject;

    std::vector<std::string> result;
    int error_count{};
    int completed_count{};
    auto sub = subject.AsObservable().FlatMap<std::string>([](int value) {
        return zaf::rx::Just(std::to_string(value));
    })
    .Subscribe([&](const std::string& value) {
        result.push_back(value);
    },
    [&](const zaf::Error& error) {
        error_count++;
    },
    [&]() {
        completed_count++;
    });

    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnError(zaf::Error{ std::make_error_code(std::errc::bad_message) });

    std::vector<std::string> expected{ "2" };
    ASSERT_EQ(result, expected);
    ASSERT_EQ(completed_count, 1);
}