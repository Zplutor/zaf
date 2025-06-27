#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/single_subject.h>

TEST(RxSingleSubjectTest, OnSuccess) {

    zaf::rx::SingleSubject<int> subject;

    std::vector<int> values;
    bool on_error_called{};
    auto sub = subject.AsSingle().Subscribe(
        [&](int value) {
            values.push_back(value);
        },
        [&](const std::exception_ptr&) {
            on_error_called = true;
        });

    subject.AsObserver().OnSuccess(55);
    
    EXPECT_EQ(values, std::vector<int>{ 55 });
    EXPECT_FALSE(on_error_called);
}


TEST(RxSingleSubjectTest, OnError) {

    zaf::rx::SingleSubject<int> subject;

    bool on_success_called{};
    bool on_error_called{};
    auto sub = subject.AsSingle().Subscribe(
        [&](int value) {
            on_success_called = true;
        },
        [&](const std::exception_ptr&) {
            on_error_called = true;
        });

    subject.AsObserver().OnError(zaf::InvalidOperationError{ "error" });
    EXPECT_FALSE(on_success_called);
    EXPECT_TRUE(on_error_called);
}