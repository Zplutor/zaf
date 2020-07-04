#include <gtest/gtest.h>
#include <zaf/base/container/utility/conversion.h>
#include <zaf/base/container/utility/range.h>

TEST(Range, NormalSequence) {

    auto values = zaf::MakeVector(zaf::Range(0, 5));
    ASSERT_EQ(values, (std::vector<int>{ 0, 1, 2, 3, 4 }));

    values = zaf::MakeVector(zaf::Range(-5, 5, 2));
    ASSERT_EQ(values, (std::vector<int>{ -5, -3, -1, 1, 3 }));

    values = zaf::MakeVector(zaf::Range(3, 8, 2));
    ASSERT_EQ(values, (std::vector<int>{ 3, 5, 7 }));

    values = zaf::MakeVector(zaf::Range(10, 3, -1));
    ASSERT_EQ(values, (std::vector<int>{ 10, 9, 8, 7, 6, 5, 4 }));

    values = zaf::MakeVector(zaf::Range(1, 1));
    ASSERT_EQ(values, (std::vector<int>{ }));

    values = zaf::MakeVector(zaf::Range(-5, -5, -2));
    ASSERT_EQ(values, (std::vector<int>{ }));
}


TEST(Range, OmitBegin) {

    auto values = zaf::MakeVector(zaf::Range(6));
    ASSERT_EQ(values, (std::vector<int>{ 0, 1, 2, 3, 4, 5 }));
}


TEST(Range, InvalidInput) {

    ASSERT_THROW(zaf::Range(4, 1), std::logic_error);
    ASSERT_THROW(zaf::Range(10, 20, -3), std::logic_error);
}