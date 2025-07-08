#include <gtest/gtest.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/single.h>
#include <zaf/rx/once.h>

TEST(RxJustTest, Observable) {

    auto observable = zaf::Observable<std::string>::Just("Just");

    std::string result;
    bool on_error_called{};
    bool on_completed_called{};
    auto sub = observable.Subscribe(
        [&result](const std::string& string) {
            result = string;
        },
        [&](std::exception_ptr) {
            on_error_called = true;
        },
        [&]() {
            on_completed_called = true;
        });

    ASSERT_EQ(result, "Just");
    ASSERT_FALSE(on_error_called);
    ASSERT_TRUE(on_completed_called);
}


TEST(RxJustTest, Single) {

    auto single = zaf::rx::Single<int>::Just(59);

    int on_success_value{};
    bool on_error_called{};
    auto sub = single.Subscribe(
        [&on_success_value](int value) {
            on_success_value = value;
        },
        [&](std::exception_ptr) {
            on_error_called = true;
        });

    ASSERT_EQ(on_success_value, 59);
    ASSERT_FALSE(on_error_called);
}


TEST(RxJustTest, Once) {

    auto once = zaf::rx::Once<int>::Just(52);
    int on_done_value{};
    auto sub = once.Subscribe(
        [&on_done_value](int value) {
            on_done_value = value;
        });
    ASSERT_EQ(on_done_value, 52);
}