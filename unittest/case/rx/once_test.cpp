#pragma once

#include <gtest/gtest.h>
#include <zaf/rx/once.h>

TEST(RxOnceTest, Just) {

    auto once = zaf::rx::Once<int>::Just(25);
    int on_done_value{};
    auto sub = once.Subscribe([&on_done_value](int value) {
        on_done_value = value;
    });
    ASSERT_EQ(on_done_value, 25);
}


TEST(RxOnceTest, Never) {
    
    auto once = zaf::rx::Once<int>::Never();
    bool on_done_called = false;
    auto sub = once.Subscribe([&on_done_called](int) {
        on_done_called = true;
    });
    ASSERT_FALSE(on_done_called);
}


TEST(RxOnceTest, ConvertToSingle) {

    auto once = zaf::rx::Once<int>::Just(31);
    zaf::rx::Single<int> single = once;
    
    int on_success_value{};
    bool on_error_called{};
    auto sub = single.Subscribe(
        [&on_success_value](int value) {
            on_success_value = value;
        },
        [&on_error_called](std::exception_ptr) {
            on_error_called = true;
        });
    ASSERT_EQ(on_success_value, 31);
    ASSERT_FALSE(on_error_called);
}


TEST(RxOnceTest, ConvertToObservable) {

    auto once = zaf::rx::Once<int>::Just(32);
    zaf::Observable<int> observable = once;

    int on_next_value{};
    bool on_error_called{};
    bool on_completed_called{};
    auto sub = observable.Subscribe(
        [&on_next_value](int value) {
            on_next_value = value;
        },
        [&on_error_called](std::exception_ptr) {
            on_error_called = true;
        },
        [&on_completed_called]() {
            on_completed_called = true;
        });
    ASSERT_EQ(on_next_value, 32);
    ASSERT_FALSE(on_error_called);
    ASSERT_TRUE(on_completed_called);
}


TEST(RxOnceTest, Do) {

    auto once = zaf::rx::Once<int>::Just(34);
    int on_done_value{};
    auto sub = once.Do([&](int value) {
        on_done_value = value;
    }).Subscribe();
    ASSERT_EQ(on_done_value, 34);
}


TEST(RxOnceTest, DoOnTerminate) {

    auto once = zaf::rx::Once<int>::Just(39);
    bool on_terminated_called = false;
    auto sub = once.DoOnTerminate([&]() {
        on_terminated_called = true;
    }).Subscribe();
    ASSERT_TRUE(on_terminated_called);
}


TEST(RxOnceTest, Map) {

    auto once = zaf::rx::Once<int>::Just(36);
    std::string mapped_value;
    auto sub = once.Map<std::string>([](int value) {
        return std::to_string(value);
    }).Subscribe([&mapped_value](const std::string& value) {
        mapped_value = value;
    });
    ASSERT_EQ(mapped_value, "36");
}


TEST(RxOnceTest, FlatMap) {

    auto once = zaf::rx::Once<int>::Just(38);
    std::string mapped_value;
    auto sub = once.FlatMap<std::string>([](int value) {
        return zaf::rx::Once<std::string>::Just(std::to_string(value));
    }).Subscribe([&mapped_value](const std::string& value) {
        mapped_value = value;
    });
    ASSERT_EQ(mapped_value, "38");
}


TEST(RxOnceTest, Finally) {

    // Finally with OnDone
    {
        auto once = zaf::rx::Once<int>::Just(36);
        bool finally_called = false;
        auto sub = once.Finally([&]() {
            finally_called = true;
        }).Subscribe();
        ASSERT_TRUE(finally_called);
    }

    // Finally on unsubscribe
    {
        auto once = zaf::rx::Once<int>::Never();
        bool finally_called = false;
        {
            auto sub = once.Finally([&]() {
                finally_called = true;
            }).Subscribe();
        }
        ASSERT_TRUE(finally_called);
    }
}