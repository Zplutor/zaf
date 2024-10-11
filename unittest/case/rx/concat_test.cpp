#include <gtest/gtest.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/subject.h>

using namespace zaf;
using namespace zaf::rx;

TEST(RxConcatTest, EmptyObservable) {

    std::vector<Observable<int>> container;
    auto observable = Concat<int>(container);

    bool on_next_called{};
    bool on_error_called{};
    bool on_completed_called{};

    auto sub = observable.Subscribe(
        [&](int) { on_next_called = true; },
        [&](std::exception_ptr) { on_error_called = true; },
        [&]() { on_completed_called = true; });

    ASSERT_FALSE(on_next_called);
    ASSERT_FALSE(on_error_called);
    ASSERT_TRUE(on_completed_called);
}


TEST(RxConcatTest, OneObservable) {

    ReplaySubject<int> subject;
    subject.AsObserver().OnNext(0);
    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnCompleted();

    auto observable = Concat<int>(std::vector{ subject.AsObservable() });

    std::vector<int> on_next_values;
    bool on_error_called{};
    bool on_completed_called{};

    auto sub = observable.Subscribe(
        [&](int value) { on_next_values.push_back(value); },
        [&](std::exception_ptr) { on_error_called = true; },
        [&]() { on_completed_called = true; });

    std::vector<int> expected{ 0, 1, 2 };
    ASSERT_EQ(on_next_values, expected);
    ASSERT_FALSE(on_error_called);
    ASSERT_TRUE(on_completed_called);
}


TEST(RxConcatTest, MultipleObservables) {

    ReplaySubject<int> subject1;
    subject1.AsObserver().OnNext(0);
    subject1.AsObserver().OnNext(1);
    subject1.AsObserver().OnNext(2);
    subject1.AsObserver().OnCompleted();

    ReplaySubject<int> subject2;
    subject2.AsObserver().OnNext(4);
    subject2.AsObserver().OnCompleted();

    ReplaySubject<int> subject3;
    subject3.AsObserver().OnNext(10);
    subject3.AsObserver().OnNext(11);
    subject3.AsObserver().OnCompleted();

    auto observable = Concat<int>(std::vector{ 
        subject1.AsObservable(),
        subject2.AsObservable(), 
        subject3.AsObservable(),
    });

    std::vector<int> on_next_values;
    bool on_error_called{};
    bool on_completed_called{};

    auto sub = observable.Subscribe(
        [&](int value) { on_next_values.push_back(value); },
        [&](std::exception_ptr) { on_error_called = true; },
        [&]() { on_completed_called = true; });

    std::vector<int> expected{ 0, 1, 2, 4, 10, 11 };
    ASSERT_EQ(on_next_values, expected);
    ASSERT_FALSE(on_error_called);
    ASSERT_TRUE(on_completed_called);
}


TEST(RxConcatTest, NoOnCompleted) {

    ReplaySubject<int> subject1;
    subject1.AsObserver().OnNext(0);
    subject1.AsObserver().OnNext(1);

    ReplaySubject<int> subject2;
    subject2.AsObserver().OnNext(4);
    subject2.AsObserver().OnCompleted();

    auto observable = Concat<int>(std::vector{ subject1.AsObservable(), subject2.AsObservable() });

    std::vector<int> on_next_values;
    bool on_error_called{};
    bool on_completed_called{};

    auto sub = observable.Subscribe(
        [&](int value) { on_next_values.push_back(value); },
        [&](std::exception_ptr) { on_error_called = true; },
        [&]() { on_completed_called = true; });

    std::vector<int> expected{ 0, 1 };
    ASSERT_EQ(on_next_values, expected);
    ASSERT_FALSE(on_error_called);
    ASSERT_FALSE(on_completed_called);
}


TEST(RxConcatTest, OnError) {

    ReplaySubject<int> subject1;
    subject1.AsObserver().OnNext(0);
    subject1.AsObserver().OnError(std::string{});

    ReplaySubject<int> subject2;
    subject2.AsObserver().OnNext(4);
    subject2.AsObserver().OnCompleted();

    auto observable = Concat<int>(std::vector{ subject1.AsObservable(), subject2.AsObservable() });

    std::vector<int> on_next_values;
    bool on_error_called{};
    bool on_completed_called{};

    auto sub = observable.Subscribe(
        [&](int value) { on_next_values.push_back(value); },
        [&](std::exception_ptr) { on_error_called = true; },
        [&]() { on_completed_called = true; });

    std::vector<int> expected{ 0 };
    ASSERT_EQ(on_next_values, expected);
    ASSERT_TRUE(on_error_called);
    ASSERT_FALSE(on_completed_called);
}


//Make sure the initializer list can be used.
TEST(RxConcatTest, InitializerList) {

    ReplaySubject<int> subject1;
    subject1.AsObserver().OnNext(0);
    subject1.AsObserver().OnCompleted();

    ReplaySubject<int> subject2;
    subject1.AsObserver().OnNext(0);
    subject1.AsObserver().OnCompleted();

    auto observable = Concat({ subject1.AsObservable(), subject2.AsObservable() });
    std::vector<int> on_next_values;

    auto sub = observable.Subscribe([&](int value) { on_next_values.push_back(value); });
    std::vector<int> expected{ 0, 0 };
    ASSERT_EQ(on_next_values, expected);
}