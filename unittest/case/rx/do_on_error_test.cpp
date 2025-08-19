#include <gtest/gtest.h>
#include <optional>
#include <zaf/rx/subjects/subject.h>

using namespace zaf;

TEST(RxDoOnErrorTest, NonTemplate) {

    int sequence{};
    int on_next_sequence{};
    int on_error_sequence{};
    int do_error_sequence{};

    rx::Subject<int> subject;
    auto sub = subject.AsObservable().DoOnError(
        [&](const std::exception_ptr& exception) {
            do_error_sequence = ++sequence;
        })
    .Subscribe(
        [&](int) {
            on_next_sequence = ++sequence;
        }, 
        [&](const std::exception_ptr& exception) {
            on_error_sequence = ++sequence;
        });

    subject.AsObserver().OnNext(11);
    subject.AsObserver().OnError(std::logic_error(""));

    ASSERT_EQ(on_next_sequence, 1);
    ASSERT_EQ(do_error_sequence, 2);
    ASSERT_EQ(on_error_sequence, 3);
}


TEST(RxDoOnErrorTest, Template) {

    //Emit an error that matches the type of DoOnError.
    {
        std::optional<std::logic_error> error;

        rx::Subject<int> subject;
        auto sub = subject.AsObservable().DoOnError<std::logic_error>(
            [&](const std::logic_error& logic_error) {
                error = logic_error;
            }).Subscribe();

        subject.AsObserver().OnError(std::logic_error("DoOnError"));
        ASSERT_TRUE(error.has_value());
        ASSERT_STREQ(error->what(), "DoOnError");
    }

    //Emit an error that doesn't match the type of DoOnError.
    {
        bool do_on_error_called{};
        rx::Subject<int> subject;
        auto sub = subject.AsObservable().DoOnError<std::logic_error>(
            [&](const std::logic_error& logic_error) {
                do_on_error_called = true;
            }).Subscribe();

        subject.AsObserver().OnError(std::runtime_error("DoOnError"));
        ASSERT_FALSE(do_on_error_called);
    }
}