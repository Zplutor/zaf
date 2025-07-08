#include <gtest/gtest.h>
#include <zaf/rx/observable.h>

TEST(RxEmptyTest, Observable) {

    auto observable = zaf::Observable<int>::Empty();

    int on_next_count{};
    int on_error_count{};
    int on_completed_count{};

    auto sub = observable.Subscribe([&](int value) {
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
    ASSERT_EQ(on_completed_count, 1);
}