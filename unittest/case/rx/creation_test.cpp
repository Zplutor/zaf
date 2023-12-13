#include <gtest/gtest.h>
#include <optional>
#include <zaf/application.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/scheduler.h>

TEST(RxEmptyTest, Normal) {

    auto observable = zaf::rx::Empty<int>();

    int on_next_count{};
    int on_error_count{};
    int on_completed_count{};

    auto sub = observable.Subscribe([&](int value) {
        ++on_next_count;
    }, 
    [&](const zaf::Error&) {
        ++on_error_count;
    },
    [&]() {
        ++on_completed_count;
    });

    ASSERT_EQ(on_next_count, 0);
    ASSERT_EQ(on_error_count, 0);
    ASSERT_EQ(on_completed_count, 1);
}


TEST(RxNeverTest, Normal) {

    auto observable = zaf::rx::Never<int>();

    int on_next_count{};
    int on_error_count{};
    int on_completed_count{};

    auto sub = observable.Subscribe([&](int value) {
        ++on_next_count;
    },
    [&](const zaf::Error&) {
        ++on_error_count;
    },
    [&]() {
        ++on_completed_count;
    });

    ASSERT_EQ(on_next_count, 0);
    ASSERT_EQ(on_error_count, 0);
    ASSERT_EQ(on_completed_count, 0);
}


TEST(RxThrowTest, Normal) {

    auto error_code = make_error_code(zaf::BasicErrc::InvalidCast);
    zaf::Error error{ error_code };
    auto observable = zaf::rx::Throw<int>(error);

    int on_next_count{};
    std::error_code catched_error_code;
    int on_completed_count{};

    auto sub = observable.Subscribe([&](int value) {
        ++on_next_count;
    },
    [&](const zaf::Error& error) {
        catched_error_code = error.Code();
    },
    [&]() {
        ++on_completed_count;
    });

    ASSERT_EQ(on_next_count, 0);
    ASSERT_EQ(catched_error_code, error_code);
    ASSERT_EQ(on_completed_count, 0);
}


TEST(RxJustTest, Normal) {

    auto observable = zaf::rx::Just(std::string{ "Just" });

    std::string result;
    zaf::Application::Instance().Subscriptions() += observable.Subscribe(
        [&result](const std::string& string) {
    
        result = string;
    });

    ASSERT_EQ(result, "Just");
}


TEST(RxCreateTest, Normal) {

    auto observable = zaf::rx::Create<int>([](zaf::Observer<int> observer) {
    
        observer.OnNext(6);
        observer.OnNext(1);
        observer.OnNext(5);
        observer.OnCompleted();
        return zaf::Subscription{};
    });

    std::vector<int> result;
    zaf::Application::Instance().Subscriptions() += observable.Subscribe([&result](int value) {
        result.push_back(value);
    });

    std::vector<int> expected{ 6, 1, 5 };
    ASSERT_EQ(result, expected);
}


TEST(RxCreateTest, NoCancelToken) {

    auto observable = zaf::rx::Create<int>(
        zaf::Scheduler::CreateOnSingleThread(), 
        [](zaf::Observer<int> observer) {

        observer.OnNext(9);
        observer.OnNext(8);
        observer.OnCompleted();
    });

    std::condition_variable cv;
    std::mutex lock;
    std::unique_lock<std::mutex> unique_lock{ lock };

    std::vector<int> result; 
    zaf::Application::Instance().Subscriptions() += observable.Subscribe([&result](int value) {
        result.push_back(value);
    }, 
    [&cv, &lock]() {
        std::scoped_lock<std::mutex> lock_guard(lock);
        cv.notify_all();
    });

    cv.wait(unique_lock);

    std::vector<int> expected{ 9, 8 };
    ASSERT_EQ(result, expected);
}


TEST(RxCreateTest, Error) {

    auto observable = zaf::rx::Create<int>([](zaf::Observer<int> observer) {

        throw zaf::Error{ std::make_error_code(std::errc::bad_message), "hello" };
        return zaf::Subscription{};
    });

    std::optional<zaf::Error> result_error;
    zaf::Application::Instance().Subscriptions() += observable.Subscribe([](int value) {

    }, 
    [&result_error](const zaf::Error& error) {
        result_error = error;
    });

    ASSERT_TRUE(result_error.has_value());
    ASSERT_EQ(result_error->Code(), std::errc::bad_message);
    ASSERT_EQ(result_error->Message(), "hello");
}