#include <gtest/gtest.h>
#include <zaf/rx/subject.h>

using namespace zaf;
using namespace zaf::rx;

//Make sure the five overloads work correctly.
TEST(RxObserverTest, Create) {

    //No arguments.
    {
        auto observer = Observer<int>::Create();
        ASSERT_NO_THROW(observer.OnNext(1));
        ASSERT_NO_THROW(observer.OnCompleted());
    }

    {
        auto observer = Observer<int>::Create();
        ASSERT_NO_THROW(observer.OnError(std::logic_error("")));
    }

    //OnNext only.
    {
        int on_next_value{};
        auto observer = Observer<int>::Create([&](int value) {
            on_next_value = value;
        });
        observer.OnNext(1);
        ASSERT_EQ(on_next_value, 1);
        ASSERT_NO_THROW(observer.OnCompleted());
    }

    {
        auto observer = Observer<int>::Create([](int) {});
        ASSERT_NO_THROW(observer.OnError(std::logic_error("")));
    }

    //OnNext, OnError.
    {
        int on_next_value{};
        double on_error_value{};
        auto observer = Observer<int>::Create(
            [&](int value) {
                on_next_value = value;
            },
            [&](const std::exception_ptr& error) {
                try {
                    std::rethrow_exception(error);
                }
                catch (double double_value) {
                    on_error_value = double_value;
                }
            });

        observer.OnNext(2);
        ASSERT_EQ(on_next_value, 2);
        observer.OnError(double(2.2));
        ASSERT_EQ(on_error_value, double(2.2));
    }

    {
        auto observer = Observer<int>::Create([](int) {}, [](const std::exception_ptr&) {});
        ASSERT_NO_THROW(observer.OnCompleted());
    }

    //OnNext, OnCompleted.
    {
        int on_next_value{};
        bool on_completed{};
        auto observer = Observer<int>::Create(
            [&](int value) {
                on_next_value = value;
            },
            [&]() {
                on_completed = true;
            });

        observer.OnNext(3);
        ASSERT_EQ(on_next_value, 3);

        observer.OnCompleted();
        ASSERT_TRUE(on_completed);
    }

    {
        auto observer = Observer<int>::Create([](int) {}, []() {});
        ASSERT_NO_THROW(observer.OnError(float(3.14)));
    }

    //OnNext, OnError, OnCompleted
    {
        int on_next_value{};
        double on_error_value{};
        auto observer = Observer<int>::Create(
            [&](int value) {
                on_next_value = value;
            },
            [&](const std::exception_ptr& error) {
                try {
                    std::rethrow_exception(error);
                }
                catch (double double_value) {
                    on_error_value = double_value;
                }
            },
            [&]() { });

        observer.OnNext(4);
        ASSERT_EQ(on_next_value, 4);

        observer.OnError(double(4.4));
        ASSERT_EQ(on_error_value, double(4.4));
    }

    {
        bool on_completed{};
        auto observer = Observer<int>::Create(
            [](int value) {},
            [](const std::exception_ptr&) { },
            [&]() {
                on_completed = true;
            });

        observer.OnCompleted();
        ASSERT_TRUE(on_completed);
    }
}


TEST(RxObserverTest, OnError) {

    std::string string_error;
    double double_error{};
    std::shared_ptr<zaf::rx::Disposable> sub;

    auto create_observer = [&]() {
        rx::Subject<int> subject;
        sub = subject.AsObservable().Subscribe(
            [](int) {},
            [&string_error, &double_error](const std::exception_ptr& exception) {
                try {
                    std::rethrow_exception(exception);
                }
                catch (const std::string& string) {
                    string_error = string;
                }
                catch (double d) {
                    double_error = d;
                }
            });
        return subject.AsObserver();
    };

    //Call the non-template overload.
    create_observer().OnError(std::make_exception_ptr(std::string("error")));
    ASSERT_EQ(string_error, "error");

    //Call the template overload.
    create_observer().OnError(double{ 3.14 });
    ASSERT_EQ(double_error, 3.14);
}