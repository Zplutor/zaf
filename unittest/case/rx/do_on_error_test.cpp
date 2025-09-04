#include <gtest/gtest.h>
#include <optional>
#include <zaf/base/error/invalid_data_error.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/subject/subject.h>

using namespace zaf;

TEST(RxDoOnErrorTest, NonTemplate_Precondition) {
    rx::Subject<int> subject;
    auto observable = subject.AsObservable();
    ASSERT_THROW(observable.DoOnError(nullptr), zaf::PreconditionError);
}


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


TEST(RxDoOnErrorTest, NonTemplate_Throw) {

    rx::Subject<int> subject;
    bool error_caught{};
    auto sub = subject.AsObservable().DoOnError([&](const std::exception_ptr& exception) {
        throw zaf::InvalidDataError{};
    })
    .Subscribe(
        [](int) {},
        [&](const std::exception_ptr& exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const zaf::InvalidDataError&) {
                error_caught = true;
            }
            catch (...) {
            }
        }
    );
    subject.AsObserver().OnError(zaf::InvalidOperationError{});
    ASSERT_TRUE(error_caught);
}


TEST(RxDoOnErrorTest, Template_Precondition) {
    rx::Subject<int> subject;
    auto observable = subject.AsObservable();
    ASSERT_THROW(observable.DoOnError<std::bad_alloc>(nullptr), zaf::PreconditionError);
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


TEST(RxDoOnErrorTest, Template_Throw) {

    rx::Subject<int> subject;
    bool error_caught{};
    auto sub = subject.AsObservable().DoOnError<zaf::InvalidOperationError>(
        [&](const zaf::InvalidOperationError& error) {
            throw zaf::InvalidDataError{};
        }
    )
    .Subscribe(
        [](int) {},
        [&](const std::exception_ptr& exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const zaf::InvalidDataError&) {
                error_caught = true;
            }
            catch (...) {
            }
        }
    );
    subject.AsObserver().OnError(zaf::InvalidOperationError{});
    ASSERT_TRUE(error_caught);
}