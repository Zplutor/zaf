#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/single.h>

TEST(RxSingleTest, Just) {

    auto single = zaf::rx::Single<int>::Just(59);

    int on_success_value{};
    auto sub = single.Subscribe([&on_success_value](int value) {
        on_success_value = value;
    });

    ASSERT_EQ(on_success_value, 59);
}


TEST(RxSingleTest, Throw) {

    // Test with std::exception_ptr
    {
        auto exception_ptr = std::make_exception_ptr(zaf::InvalidOperationError{ "single.throw"});
        auto single = zaf::rx::Single<int>::Throw(exception_ptr);

        bool on_success_called{};
        std::string error_message;
        auto sub = single.Subscribe(
            [&](int) { on_success_called = true; },
            [&error_message](std::exception_ptr exception) { 
                try {
                    std::rethrow_exception(exception);
                } 
                catch (const zaf::InvalidOperationError& error) {
                    error_message = error.what();
                }
            });

        ASSERT_FALSE(on_success_called);
        ASSERT_EQ(error_message, "single.throw");
    }

    // Test with exception object
    {
        auto single = zaf::rx::Single<int>::Throw(zaf::InvalidOperationError{ "single.throw2" });

        bool on_success_called{};
        std::string error_message;
        auto sub = single.Subscribe(
            [&](int) { on_success_called = true; },
            [&error_message](std::exception_ptr exception) { 
                try {
                    std::rethrow_exception(exception);
                } 
                catch (const zaf::InvalidOperationError& error) {
                    error_message = error.what();
                }
            });
        ASSERT_FALSE(on_success_called);
        ASSERT_EQ(error_message, "single.throw2");
    }
}


TEST(RxSingleTest, Never) {

    auto single = zaf::rx::Single<int>::Never();

    bool on_success_called{};
    bool on_error_called{};
    auto sub = single.Subscribe(
        [&](int) { on_success_called = true; },
        [&](std::exception_ptr) { on_error_called = true; });

    ASSERT_FALSE(on_success_called);
    ASSERT_FALSE(on_error_called);
}


TEST(RxSingleTest, ConvertToObservable) {

    //OnSuccess
    {
        auto single = zaf::rx::Single<int>::Just(34);

        zaf::Observable<int> observable = single;

        int on_next_value{};
        bool on_error_called{};
        bool on_completed_called{};
        auto sub = observable.Subscribe(
            [&](int value) { on_next_value = value; },
            [&](std::exception_ptr) { on_error_called = true; },
            [&]() { on_completed_called = true; });

        ASSERT_EQ(on_next_value, 34);
        ASSERT_FALSE(on_error_called);
        ASSERT_TRUE(on_completed_called);
    }

    //OnError
    {
        auto single = zaf::rx::Single<int>::Throw(zaf::InvalidOperationError{ "convert.error" });
        zaf::Observable<int> observable = single;

        bool on_next_called{};
        std::string error_message;
        bool on_completed_called{};
        auto sub = observable.Subscribe(
            [&](int) { on_next_called = true; },
            [&error_message](std::exception_ptr exception) { 
                try {
                    std::rethrow_exception(exception);
                } 
                catch (const zaf::InvalidOperationError& error) {
                    error_message = error.what();
                }
            },
            [&]() { on_completed_called = true; });

        ASSERT_FALSE(on_next_called);
        ASSERT_EQ(error_message, "convert.error");
        ASSERT_FALSE(on_completed_called);
    }
}