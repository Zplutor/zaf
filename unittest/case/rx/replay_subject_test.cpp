#include <mutex>
#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/replay_subject.h>

static_assert(!std::is_copy_assignable_v<zaf::rx::ReplaySubject<int>>);
static_assert(!std::is_copy_constructible_v<zaf::rx::ReplaySubject<int>>);
static_assert(std::is_move_assignable_v<zaf::rx::ReplaySubject<int>>);
static_assert(std::is_move_constructible_v<zaf::rx::ReplaySubject<int>>);


TEST(RxReplaySubjectTest, Replay) {

    zaf::rx::ReplaySubject<int> subject;

    auto observer = subject.AsObserver();
    observer.OnNext(8);
    observer.OnNext(7);
    observer.OnNext(9);
    observer.OnNext(1);

    std::vector<int> sequence;
    bool has_error{};
    bool is_completed{};
    auto subscription = subject.AsObservable().Subscribe([&sequence](int value) {
        sequence.push_back(value);
    },
    [&has_error](const std::exception_ptr& error) {
        has_error = true;
    },
    [&is_completed]() {
        is_completed = true;
    });

    std::vector<int> expected{ 8, 7, 9, 1 };
    ASSERT_EQ(sequence, expected);
    ASSERT_FALSE(has_error);
    ASSERT_FALSE(is_completed);

    observer.OnNext(5);
    observer.OnNext(0);
    observer.OnCompleted();

    expected.push_back(5);
    expected.push_back(0);
    ASSERT_EQ(sequence, expected);
    ASSERT_FALSE(has_error);
    ASSERT_TRUE(is_completed);
}


TEST(RxReplaySubjectTest, ReplayWithSize) {

    //Zero size.
    {
        zaf::rx::ReplaySubject<int> subject{ 0 };
        auto observer = subject.AsObserver();
        observer.OnNext(1);
        observer.OnNext(2);
        observer.OnNext(3);
        std::vector<int> sequence;
        auto subscription = subject.AsObservable().Subscribe([&sequence](int value) {
            sequence.push_back(value);
        });
        std::vector<int> expected{};
        ASSERT_EQ(sequence, expected);
    }

    //None zero size.
    {
        zaf::rx::ReplaySubject<int> subject{ 3 };
        auto observer = subject.AsObserver();
        observer.OnNext(1);
        observer.OnNext(2);
        observer.OnNext(3);
        observer.OnNext(4);
        observer.OnNext(5);

        std::vector<int> sequence;
        auto subscription = subject.AsObservable().Subscribe([&sequence](int value) {
            sequence.push_back(value);
        });

        std::vector<int> expected{ 3, 4, 5 };
        ASSERT_EQ(sequence, expected);
    }
}


TEST(RxReplaySubjectTest, ReplayError) {

    zaf::rx::ReplaySubject<int> subject;

    auto observer = subject.AsObserver();
    observer.OnNext(8);
    observer.OnNext(7);
    observer.OnError(std::make_exception_ptr(9));

    std::vector<int> sequence;
    bool has_error{};
    bool is_completed{};
    auto subscription = subject.AsObservable().Subscribe([&sequence](int value) {
        sequence.push_back(value);
    },
    [&has_error](const std::exception_ptr& error) {
        has_error = true;
    },
    [&is_completed]() {
        is_completed = true;
    });

    std::vector<int> expected{ 8, 7 };
    ASSERT_EQ(sequence, expected);
    ASSERT_TRUE(has_error);
    ASSERT_FALSE(is_completed);
}


TEST(RxReplaySubjectTest, EmitAfterTermination) {

    //Emit after normal termination.
    {
        zaf::rx::ReplaySubject<int> subject;
        auto observer = subject.AsObserver();
        observer.OnNext(1);
        observer.OnNext(2);
        observer.OnCompleted();
        //The following emissions will be ignored.
        observer.OnNext(3);
        observer.OnError(zaf::InvalidOperationError{});
        observer.OnCompleted();

        std::vector<int> sequence;
        int on_error_count{};
        int on_completed_count{};
        auto subscription = subject.AsObservable().Subscribe([&sequence](int value) {
            sequence.push_back(value);
        },
        [&on_error_count](const std::exception_ptr& error) {
            on_error_count++;
        },
        [&on_completed_count]() {
            on_completed_count++;
        });
        std::vector<int> expected{ 1, 2 };
        ASSERT_EQ(sequence, expected);
        ASSERT_EQ(on_error_count, 0);
        ASSERT_EQ(on_completed_count, 1);
    }

    //Emit after error termination.
    {
        zaf::rx::ReplaySubject<int> subject;
        auto observer = subject.AsObserver();
        observer.OnNext(1);
        observer.OnNext(2);
        observer.OnError(zaf::InvalidOperationError{});
        //The following emissions will be ignored.
        observer.OnNext(3);
        observer.OnError(zaf::InvalidOperationError{});
        observer.OnCompleted();

        std::vector<int> sequence;
        int on_error_count{};
        int on_completed_count{};
        auto subscription = subject.AsObservable().Subscribe([&sequence](int value) {
            sequence.push_back(value);
        },
        [&on_error_count](const std::exception_ptr& error) {
            on_error_count++;
        },
        [&on_completed_count]() {
            on_completed_count++;
        });
        std::vector<int> expected{ 1, 2 };
        ASSERT_EQ(sequence, expected);
        ASSERT_EQ(on_error_count, 1);
        ASSERT_EQ(on_completed_count, 0);
    }
}


//The subject is destroyed during the emission. Make sure it is handled properly.
TEST(RxReplaySubjectTest, DestroySubjectDuringEmission) {

    std::optional<zaf::rx::ReplaySubject<int>> subject;
    subject.emplace();

    subject->AsObserver().OnNext(0);
    subject->AsObserver().OnNext(1);
    subject->AsObserver().OnNext(2);
    subject->AsObserver().OnCompleted();

    auto observable = subject->AsObservable();
    auto sub = observable.Subscribe([&](int value) {
        if (value == 1) {
            subject.reset();
            observable = zaf::Observable<int>::Empty();
        }
    });
}