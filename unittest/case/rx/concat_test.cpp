#include <gtest/gtest.h>
#include <zaf/rx/replay_subject.h>
#include <zaf/rx/single_subject.h>
#include <zaf/rx/subject.h>

using namespace zaf;
using namespace zaf::rx;

TEST(RxConcatTest, EmptyObservable) {

    std::vector<Observable<int>> container;
    auto observable = Observable<int>::Concat(container);

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

    zaf::rx::ReplaySubject<int> subject;
    subject.AsObserver().OnNext(0);
    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnCompleted();

    auto observable = Observable<int>::Concat(std::vector{ subject.AsObservable() });

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

    zaf::rx::ReplaySubject<int> subject1;
    subject1.AsObserver().OnNext(0);
    subject1.AsObserver().OnNext(1);
    subject1.AsObserver().OnNext(2);
    subject1.AsObserver().OnCompleted();

    zaf::rx::ReplaySubject<int> subject2;
    subject2.AsObserver().OnNext(4);
    subject2.AsObserver().OnCompleted();

    zaf::rx::ReplaySubject<int> subject3;
    subject3.AsObserver().OnNext(10);
    subject3.AsObserver().OnNext(11);
    subject3.AsObserver().OnCompleted();

    auto observable = Observable<int>::Concat(std::vector{ 
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

    zaf::rx::ReplaySubject<int> subject1;
    subject1.AsObserver().OnNext(0);
    subject1.AsObserver().OnNext(1);

    zaf::rx::ReplaySubject<int> subject2;
    subject2.AsObserver().OnNext(4);
    subject2.AsObserver().OnCompleted();

    auto observable = Observable<int>::Concat(std::vector{
        subject1.AsObservable(), 
        subject2.AsObservable() 
    });

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

    zaf::rx::ReplaySubject<int> subject1;
    subject1.AsObserver().OnNext(0);
    subject1.AsObserver().OnError(std::string{});

    zaf::rx::ReplaySubject<int> subject2;
    subject2.AsObserver().OnNext(4);
    subject2.AsObserver().OnCompleted();

    auto observable = Observable<int>::Concat(std::vector{ 
        subject1.AsObservable(),
        subject2.AsObservable() 
    });

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
TEST(RxConcatTest, ObservableInitializerList) {

    zaf::rx::ReplaySubject<int> subject1;
    subject1.AsObserver().OnNext(0);
    subject1.AsObserver().OnCompleted();

    zaf::rx::ReplaySubject<int> subject2;
    subject2.AsObserver().OnNext(0);
    subject2.AsObserver().OnCompleted();

    auto observable = Observable<int>::Concat({ 
        subject1.AsObservable(), 
        subject2.AsObservable() 
    });
    std::vector<int> on_next_values;

    auto sub = observable.Subscribe([&](int value) { on_next_values.push_back(value); });
    std::vector<int> expected{ 0, 0 };
    ASSERT_EQ(on_next_values, expected);
}


TEST(RxConcatTest, SingleRange) {

    SingleSubject<int> single1;
    SingleSubject<int> single2;

    std::vector<Single<int>> singles{
        single1.AsSingle(),
        single2.AsSingle(),
    };

    std::vector<int> values;
    bool on_completed_called{};
    auto sub = Observable<int>::Concat(singles).Subscribe([&](int value) {
        values.push_back(value);
    }, 
    [&]() {
        on_completed_called = true;
    });

    single1.AsObserver().OnSuccess(28);
    single2.AsObserver().OnSuccess(29);

    ASSERT_EQ(values, (std::vector{ 28, 29 }));
    ASSERT_TRUE(on_completed_called);
}