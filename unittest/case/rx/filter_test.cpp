#include <gtest/gtest.h>
#include <zaf/base/error/invalid_data_error.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/subject/subject.h>

TEST(RxFilterTest, Precondition) {

    auto observable = zaf::rx::Observable<int>::Just(1);
    ASSERT_THROW(observable.Filter(nullptr), zaf::PreconditionError);
}


TEST(RxFilterTest, Filter) {

    zaf::rx::Subject<int> subject;

    std::vector<int> values;
    auto sub = subject.AsObservable().Filter([](int value) {
        return value % 2 == 0; 
    })
    .Subscribe([&](int value) {
        values.push_back(value);
    });

    subject.AsObserver().OnNext(0);
    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnNext(3);
    subject.AsObserver().OnNext(4);
    subject.AsObserver().OnNext(5);
    subject.AsObserver().OnNext(6);
    subject.AsObserver().OnNext(7);
    subject.AsObserver().OnNext(8);
    subject.AsObserver().OnNext(9);

    ASSERT_EQ(values, (std::vector<int>{ 0, 2, 4, 6, 8 }));
}


TEST(RxFilterTest, ThrowInPredicate) {

    zaf::rx::Subject<int> subject;

    std::vector<int> values;
    std::exception_ptr captured_error;
    auto sub = subject.AsObservable().Filter([](int value) {
        if (value == 3) {
            throw zaf::InvalidDataError{};
        }
        return true;
    })
    .Subscribe(
        [&](int value) {
            values.push_back(value);
        },
        [&](std::exception_ptr error) { 
            captured_error = error; 
        });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnNext(2);
    subject.AsObserver().OnNext(3);
    subject.AsObserver().OnNext(4);

    ASSERT_EQ(values, (std::vector<int>{ 1, 2 }));
    ASSERT_TRUE(captured_error != nullptr);
}