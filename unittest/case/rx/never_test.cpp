#include <gtest/gtest.h>
#include <zaf/rx/continuous.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/once.h>
#include <zaf/rx/single.h>

TEST(RxNeverTest, Observable) {

    auto observable = zaf::Observable<int>::Never();

    int on_next_count{};
    int on_error_count{};
    int on_completed_count{};

    auto sub = observable.Subscribe(
        [&](int value) {
            ++on_next_count;
        },
        [&](const std::exception_ptr&) {
            ++on_error_count;
        },
        [&]() {
            ++on_completed_count;
        });

    ASSERT_EQ(on_next_count, 0);
    ASSERT_EQ(on_error_count, 0);
    ASSERT_EQ(on_completed_count, 0);
}


TEST(RxNeverTest, Single) {

    auto single = zaf::rx::Single<int>::Never();

    bool on_success_called{};
    bool on_error_called{};
    auto sub = single.Subscribe(
        [&](int) { on_success_called = true; },
        [&](std::exception_ptr) { on_error_called = true; });

    ASSERT_FALSE(on_success_called);
    ASSERT_FALSE(on_error_called);
}


TEST(RxNeverTest, Once) {

    auto once = zaf::rx::Once<int>::Never();
    bool on_done_called{};
    auto sub = once.Subscribe([&on_done_called](int) {
        on_done_called = true;
    });
    ASSERT_FALSE(on_done_called);
}


TEST(RxNeverTest, Continuous) {

    auto continuous = zaf::rx::Continuous<int>::Never();
    bool on_next_called{};
    auto sub = continuous.Subscribe([&on_next_called](int) {
        on_next_called = true;
    });
    ASSERT_FALSE(on_next_called);
}