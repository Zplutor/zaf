#include <gtest/gtest.h>
#include <zaf/rx/continuous.h>
#include <zaf/rx/continuous_subject.h>

TEST(RxContinuousTest, Never) {

    bool on_next_called{};
    auto sub = zaf::rx::Continuous<int>::Never().Subscribe([&on_next_called](const int&) {
        on_next_called = true;
    });
    ASSERT_FALSE(on_next_called);
}


TEST(RxContinuousTest, OnNext) {

    zaf::rx::ContinuousSubject<int> subject;

    std::vector<int> values;
    auto sub = subject.AsContinuous().Subscribe([&values](const int& value) {
        values.push_back(value);
    });

    subject.AsObserver().OnNext(49);
    subject.AsObserver().OnNext(50);
    subject.AsObserver().OnNext(51);
    ASSERT_EQ(values, (std::vector<int>{ 49, 50, 51 }));
}


TEST(RxContinuousTest, ConvertToObservable) {

    zaf::rx::ContinuousSubject<int> subject;
    zaf::Observable<int> observable = subject.AsContinuous();

    std::vector<int> on_next_values{};
    bool on_error_called{};
    bool on_completed_called{};
    auto sub = observable.Subscribe(
        [&](const int& value) { on_next_values.push_back(value); },
        [&](const std::exception_ptr&) { on_error_called = true; },
        [&]() { on_completed_called = true; }
    );

    subject.AsObserver().OnNext(53);
    subject.AsObserver().OnNext(54);

    ASSERT_EQ(on_next_values, (std::vector<int>{ 53, 54 }));
    ASSERT_FALSE(on_error_called);
    ASSERT_FALSE(on_completed_called);
}


TEST(RxContinuousTest, Map) {

    zaf::rx::ContinuousSubject<int> subject;

    std::vector<std::string> values;
    auto sub = subject.AsContinuous().Map<std::string>([](const int& value) {
        return std::to_string(value);
    }).Subscribe(
        [&](const std::string& value) { values.push_back(value); }
    );
    subject.AsObserver().OnNext(57);
    subject.AsObserver().OnNext(58);
    ASSERT_EQ(values, (std::vector<std::string>{ "57", "58" }));
}


TEST(RxContinuousTest, FlatMap) {

    zaf::rx::ContinuousSubject<int> subject;
    std::vector<std::string> values;
    auto sub = subject.AsContinuous().FlatMap<std::string>([](const int& value) {
        zaf::rx::ContinuousSubject<std::string> mapped;
        mapped.AsObserver().OnNext(std::to_string(value));
        return mapped.AsContinuous();
    }).Subscribe(
        [&](const std::string& value) { values.push_back(value); }
    );
    subject.AsObserver().OnNext(0);
    subject.AsObserver().OnNext(1);
    ASSERT_EQ(values, (std::vector<std::string>{ "0", "1" }));
}