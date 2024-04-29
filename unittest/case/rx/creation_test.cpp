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


TEST(RxNeverTest, Normal) {

    auto observable = zaf::rx::Never<int>();

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
    ASSERT_EQ(on_completed_count, 0);
}


TEST(RxThrowTest, Normal) {

    auto observable = zaf::rx::Throw<int>(std::make_exception_ptr(std::string("err")));

    int on_next_count{};
    std::string catched_error_string;
    int on_completed_count{};

    auto sub = observable.Subscribe([&](int value) {
        ++on_next_count;
    },
    [&](const std::exception_ptr& exception) {
        try {
            std::rethrow_exception(exception);
        }
        catch (const std::string& error_string) {
            catched_error_string = error_string;
        }
    },
    [&]() {
        ++on_completed_count;
    });

    ASSERT_EQ(on_next_count, 0);
    ASSERT_EQ(catched_error_string, "err");
    ASSERT_EQ(on_completed_count, 0);
}


//Calls the template error version of Throw().
TEST(RxThrowTest, Template) {

    auto observable = zaf::rx::Throw<double>(std::logic_error("throw logic error"));

    std::optional<std::logic_error> catched_error;

    auto sub = observable.Subscribe(
        [](double) {},
        [&catched_error](const std::exception_ptr& exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const std::logic_error& logic_error) {
                catched_error = logic_error;
            }
        });

    ASSERT_STREQ(catched_error->what(), "throw logic error");
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