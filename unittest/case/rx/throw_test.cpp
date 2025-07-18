#include <gtest/gtest.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/single.h>

TEST(RxThrowTest, Observable_ExceptionPtr) {

    auto observable = zaf::rx::Observable<int>::Throw(std::make_exception_ptr(std::string("err")));

    int on_next_count{};
    std::string caught_error_string;
    int on_completed_count{};

    auto sub = observable.Subscribe([&](int value) {
            ++on_next_count;
        },
        [&](const std::exception_ptr& exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const std::string& error) {
                caught_error_string = error;
            }
        },
        [&]() {
            ++on_completed_count;
        });

    ASSERT_EQ(on_next_count, 0);
    ASSERT_EQ(caught_error_string, "err");
    ASSERT_EQ(on_completed_count, 0);
}


TEST(RxThrowTest, Observable_Template) {

    auto observable = zaf::rx::Observable<double>::Throw(std::logic_error("throw logic error"));

    std::optional<std::logic_error> caught_error;

    auto sub = observable.Subscribe(
        [](double) {},
        [&caught_error](const std::exception_ptr& exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const std::logic_error& logic_error) {
                caught_error = logic_error;
            }
        });

    ASSERT_TRUE(caught_error.has_value());
    ASSERT_STREQ(caught_error->what(), "throw logic error");
}


TEST(RxThrowTest, Single_ExceptionPtr) {

    auto single = zaf::rx::Single<int>::Throw(std::make_exception_ptr(std::string("err")));
    std::string caught_error;
    auto sub = single.Subscribe(
        [](int) {},
        [&](const std::exception_ptr& exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const std::string& error) {
                caught_error = error;
            }
        });

    ASSERT_EQ(caught_error, "err");
}


TEST(RxThrowTest, Single_Template) {

    auto single = zaf::rx::Single<int>::Throw(std::logic_error("throw logic error"));
    std::optional<std::logic_error> caught_error;
    auto sub = single.Subscribe(
        [](int) {},
        [&](const std::exception_ptr& exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const std::logic_error& logic_error) {
                caught_error = logic_error;
            }
        });

    ASSERT_TRUE(caught_error.has_value());
    ASSERT_STREQ(caught_error->what(), "throw logic error");
}