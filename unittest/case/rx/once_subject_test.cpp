#include <gtest/gtest.h>
#include <zaf/rx/once_subject.h>

TEST(RxOnceSubjectTest, OnceSubject) {

    zaf::rx::OnceSubject<int> subject;
    int value{};
    auto sub = subject.AsOnce().Subscribe([&](int v) { value = v; });
    subject.AsObserver().OnDone(46);
    EXPECT_EQ(value, 46);
}