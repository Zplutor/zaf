#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject/subject.h>

TEST(RxSubscribeTest, Precondition) {

    auto observable = zaf::rx::Observable<int>::Just(0);

    // OnNext
    ASSERT_THROW(auto sub = observable.Subscribe(nullptr), zaf::PreconditionError);

    // OnNext + OnError
    ASSERT_THROW(
        auto sub = observable.Subscribe(nullptr, [](std::exception_ptr) {}), 
        zaf::PreconditionError);
    ASSERT_THROW(
        auto sub = observable.Subscribe([](int) {}, zaf::rx::OnError{}),
        zaf::PreconditionError);

    // OnNext + OnCompleted
    ASSERT_THROW(
        auto sub = observable.Subscribe(nullptr, []() {}), 
        zaf::PreconditionError);
    ASSERT_THROW(
        auto sub = observable.Subscribe([](int) {}, zaf::rx::OnCompleted{}), 
        zaf::PreconditionError);

    // OnNext + OnError + OnCompleted
    // This overload doesn't have precondition.
    ASSERT_NO_THROW(auto sub = observable.Subscribe(nullptr, nullptr, nullptr));
}


//Make sure there is no exception with empty observer.
TEST(RxSubscribeTest, EmptyObserver) {

    zaf::rx::Subject<int> subject;
    {
        auto subscription = subject.AsObservable().Subscribe();
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnCompleted();
    }

    {
        auto subscription = subject.AsObservable().Subscribe();
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnError(std::make_exception_ptr(7));
    }
}


// Exception thrown in OnNext is considered as an unhandled exception and won't be passed to the 
// OnError notification.
TEST(RxSubscribeTest, ThrowInOnNext) {

    std::optional<zaf::InvalidOperationError> caught_error;

    zaf::rx::Subject<int> subject;
    auto sub = subject.AsObservable().Subscribe(
        [](int value) {
            throw zaf::InvalidOperationError();
        },
        [&](const std::exception_ptr& error) {
            try {
                std::rethrow_exception(error);
            }
            catch (const zaf::InvalidOperationError& e) {
                caught_error = e;
            }
        });

    ASSERT_THROW(subject.AsObserver().OnNext(0), zaf::InvalidOperationError);
    ASSERT_FALSE(caught_error.has_value());
}