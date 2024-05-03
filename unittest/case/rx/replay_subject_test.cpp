#include <gtest/gtest.h>
#include <zaf/rx/subject.h>

static_assert(!std::is_copy_assignable_v<zaf::ReplaySubject<int>>);
static_assert(!std::is_copy_constructible_v<zaf::ReplaySubject<int>>);
static_assert(std::is_move_assignable_v<zaf::ReplaySubject<int>>);
static_assert(std::is_move_constructible_v<zaf::ReplaySubject<int>>);


TEST(RxReplaySubjectTest, Replay) {

    zaf::ReplaySubject<int> subject;

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


TEST(RxReplaySubjectTest, ReplayError) {

    zaf::ReplaySubject<int> subject;

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