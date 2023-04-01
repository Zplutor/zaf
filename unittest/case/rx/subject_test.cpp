#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/rx/subject.h>

static_assert(!std::is_copy_assignable_v<zaf::Subject<int>>);
static_assert(!std::is_copy_constructible_v<zaf::Subject<int>>);
static_assert(!std::is_move_assignable_v<zaf::Subject<int>>);
static_assert(!std::is_move_constructible_v<zaf::Subject<int>>);


TEST(RxSubjectTest, Normal) {

    zaf::Subject<int> subject;

    std::vector<int> values;
    zaf::Application::Instance().Subscriptions() += subject.GetObservable().Subscribe(
        [&values](int value) {
    
        values.push_back(value);
    });

    subject.GetObserver().OnNext(8);
    subject.GetObserver().OnNext(5); 
    subject.GetObserver().OnNext(0);
    subject.GetObserver().OnNext(3);

    std::vector<int> expected{ 8, 5, 0, 3 };
    ASSERT_EQ(values, expected);
}