#include <gtest/gtest.h>
#include <optional>
#include <zaf/application.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/observable.h>

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