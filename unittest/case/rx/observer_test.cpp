#include <gtest/gtest.h>
#include <zaf/rx/subject.h>

using namespace zaf;

TEST(RxObserverTest, OnError) {

    std::string string_error;
    double double_error{};
    Subscription sub;

    auto create_observer = [&]() {
        Subject<int> subject;
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