#include <gtest/gtest.h>
#include <zaf/rx/continuous.h>
#include <zaf/rx/continuous_subject.h>

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


TEST(RxContinuousTest, Do) {

    zaf::rx::ContinuousSubject<int> subject;
    std::vector<int> values;
    auto sub = subject.AsContinuous().Do([&](const int& value) {
        values.push_back(value);
    }).Subscribe();
    subject.AsObserver().OnNext(30);
    subject.AsObserver().OnNext(31);
    ASSERT_EQ(values, (std::vector<int>{ 30, 31 }));
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