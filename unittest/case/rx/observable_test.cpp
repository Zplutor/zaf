#include <gtest/gtest.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>

TEST(RxObservableTest, EmptyObserver) {

    //Make sure there is no excepiton with empty observer.

    zaf::Subject<int> subject;

    {
        auto subscription = subject.AsObservable().Subscribe();
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnCompleted();
    }

    {
        auto subscription = subject.AsObservable().Subscribe();
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnError(zaf::Error(std::make_error_code(std::errc::bad_address)));
    }
}