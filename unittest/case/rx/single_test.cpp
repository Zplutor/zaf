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


TEST(RxSingleTest, Do) {

    // Do with OnSuccess on success value
    {
        auto single = zaf::rx::Single<int>::Just(19);
        int on_success_value{};
        auto sub = single.Do(
            [&](int value) { on_success_value = value; }
        ).Subscribe();
        ASSERT_EQ(on_success_value, 19);
    }

    // Do with OnSuccess on error value
    {
        auto single = zaf::rx::Single<int>::Throw(zaf::InvalidOperationError{ "do.error" });
        bool on_success_called{};
        auto sub = single.Do(
            [&](int) { on_success_called = true; }
        ).Subscribe();
        ASSERT_FALSE(on_success_called);
    }

    // Do with OnSuccess and OnError on success value
    {
        auto single = zaf::rx::Single<int>::Just(17);

        int on_success_value{};
        bool on_error_called{};
        auto sub = single.Do(
            [&](int value) { on_success_value = value; },
            [&](std::exception_ptr) { on_error_called = true; }
        ).Subscribe();

        ASSERT_EQ(on_success_value, 17);
        ASSERT_FALSE(on_error_called);
    }

    // Do with OnSuccess and OnError on error value
    {
        auto single = zaf::rx::Single<int>::Throw(zaf::InvalidOperationError{ "do.error" });
        bool on_success_called{};
        std::string error_message;
        auto sub = single.Do(
            [&](int) { on_success_called = true; },
            [&error_message](std::exception_ptr exception) {
                try {
                    std::rethrow_exception(exception);
                }
                catch (const zaf::InvalidOperationError& error) {
                    error_message = error.what();
                }
            }
        ).Subscribe();
        ASSERT_FALSE(on_success_called);
        ASSERT_EQ(error_message, "do.error");
    }
}


TEST(RxSingleTest, DoOnError) {

    // DoOnError with OnSuccess
    {
        auto single = zaf::rx::Single<int>::Just(28);
        bool on_error_called = false;
        auto sub = single.DoOnError([&](std::exception_ptr) {
            on_error_called = true;
        }).Subscribe();
        ASSERT_FALSE(on_error_called);
    }

    // DoOnError with OnError
    {
        auto single = zaf::rx::Single<int>::Throw(zaf::InvalidOperationError{ "error" });
        bool on_error_called = false;
        auto sub = single.DoOnError([&](std::exception_ptr exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const zaf::InvalidOperationError&) {
                on_error_called = true;
            }
        }).Subscribe();
        ASSERT_TRUE(on_error_called);
    }
}


TEST(RxSingleTest, DoOnErrorTemplate) {

    // DoOnError with OnSuccess
    {
        auto single = zaf::rx::Single<int>::Just(29);
        bool on_error_called = false;
        auto sub = single.DoOnError<zaf::InvalidOperationError>([&](const auto&) {
            on_error_called = true;
        }).Subscribe();
        ASSERT_FALSE(on_error_called);
    }

    // DoOnError with OnError
    {
        auto single = zaf::rx::Single<int>::Throw(zaf::InvalidOperationError{ "error" });
        bool on_error_called = false;
        auto sub = single.DoOnError<zaf::InvalidOperationError>([&](const auto&) {
            on_error_called = true;
        }).Subscribe();
        ASSERT_TRUE(on_error_called);
    }

    // DoOnError with different error type
    {
        auto single = zaf::rx::Single<int>::Throw(std::runtime_error{ "runtime error" });
        bool on_error_called = false;
        auto sub = single.DoOnError<zaf::InvalidOperationError>([&](const auto&) {
            on_error_called = true;
        }).Subscribe();
        ASSERT_FALSE(on_error_called);
    }
}


TEST(RxSingleTest, DoOnTerminate) {

    // DoOnTerminate with OnSuccess
    {
        auto single = zaf::rx::Single<int>::Just(24);
        bool on_terminated_called = false;
        auto sub = single.DoOnTerminate([&]() {
            on_terminated_called = true;
        }).Subscribe();
        ASSERT_TRUE(on_terminated_called);
    }

    // DoOnTerminate with OnError
    {
        auto single = zaf::rx::Single<int>::Throw(zaf::InvalidOperationError{ "error" });
        bool on_terminated_called = false;
        auto sub = single.DoOnTerminate([&]() {
            on_terminated_called = true;
        }).Subscribe();
        ASSERT_TRUE(on_terminated_called);
    }
}


TEST(RxSingleTest, Map) {

    auto single = zaf::rx::Single<int>::Just(37);
    std::string mapped_value;
    auto sub = single.Map<std::string>([](int value) {
        return std::to_string(value);
    }).Subscribe([&mapped_value](const std::string& value) {
        mapped_value = value;
    });
    ASSERT_EQ(mapped_value, "37");
}


TEST(RxSingleTest, FlatMap) {

    auto single = zaf::rx::Single<int>::Just(51);
    std::string flat_mapped_value;
    auto sub = single.FlatMap<std::string>([](int value) {
        return zaf::rx::Single<std::string>::Just(std::to_string(value));
    }).Subscribe([&flat_mapped_value](const std::string& value) {
        flat_mapped_value = value;
    });
    ASSERT_EQ(flat_mapped_value, "51");
}