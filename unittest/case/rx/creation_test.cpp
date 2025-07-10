#include <gtest/gtest.h>
#include <optional>
#include <zaf/application.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/scheduler.h>

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
        throw std::string("hello");
        return zaf::Subscription{};
    });

    std::optional<std::string> result_error;
    zaf::Application::Instance().Subscriptions() += observable.Subscribe([](int value) {

    }, 
    [&result_error](const std::exception_ptr& error) {
        try {
            std::rethrow_exception(error);
        }
        catch (const std::string& string) {
            result_error = string;
        }
    });

    ASSERT_TRUE(result_error.has_value());
    ASSERT_EQ(*result_error, "hello");
}