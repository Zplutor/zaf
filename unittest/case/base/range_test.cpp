#include <gtest/gtest.h>
#include <zaf/base/range.h>

TEST(RangeTest, Constructor) {

    {
        zaf::Range range;
        ASSERT_EQ(range.index, 0);
        ASSERT_EQ(range.length, 0);
    }

    {
        zaf::Range range{ 4, 5 };
        ASSERT_EQ(range.index, 4);
        ASSERT_EQ(range.length, 5);
    }
}


TEST(RangeTest, Infinite) {

    auto range = zaf::Range::Infinite();
    ASSERT_EQ(range.index, 0);
    ASSERT_EQ(range.length, (std::numeric_limits<std::size_t>::max)());
}


TEST(RangeTest, FromIndexPair) {

    auto range = zaf::Range::FromIndexPair(0, 0);
    ASSERT_EQ(range.index, 0);
    ASSERT_EQ(range.length, 0);

    range = zaf::Range::FromIndexPair(5, 5);
    ASSERT_EQ(range.index, 5);
    ASSERT_EQ(range.length, 0);

    range = zaf::Range::FromIndexPair(5, 6);
    ASSERT_EQ(range.index, 5);
    ASSERT_EQ(range.length, 1);

    range = zaf::Range::FromIndexPair(5, 10);
    ASSERT_EQ(range.index, 5);
    ASSERT_EQ(range.length, 5);

    ASSERT_THROW(zaf::Range::FromIndexPair(5, 3), std::logic_error);
}


TEST(RangeTest, EndIndex) {

    zaf::Range range;
    ASSERT_EQ(range.EndIndex(), 0);

    range = zaf::Range(44, 24);
    ASSERT_EQ(range.EndIndex(), 68);

    range = zaf::Range::Infinite();
    ASSERT_EQ(range.EndIndex(), (std::numeric_limits<std::size_t>::max)());
}


TEST(RangeTest, UseInRangeFor) {

    std::vector<std::size_t> result;
    for (auto index : zaf::Range{ 0, 9 }) {
        result.push_back(index);
    }
    std::vector<std::size_t> expected{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    ASSERT_EQ(result, expected);

    result.clear();
    for (auto index : zaf::Range(5, 8)) {
        result.push_back(index);
    }
    expected = { 5, 6, 7, 8, 9, 10, 11, 12 };
    ASSERT_EQ(result, expected);
}


TEST(RangeTest, FromDWRITETEXTRANGE) {

    DWRITE_TEXT_RANGE text_range;
    text_range.startPosition = 110;
    text_range.length = 78;

    auto range = zaf::Range::FromDWRITETEXTRANGE(text_range);
    ASSERT_EQ(range.index, 110);
    ASSERT_EQ(range.length, 78);
}


TEST(RangeTest, ToDWRITETEXTRANGE) {

    zaf::Range range{ 234, 88 };
    auto text_range = range.ToDWRITETEXTRANGE();
    ASSERT_EQ(text_range.startPosition, 234);
    ASSERT_EQ(text_range.length, 88);
}


TEST(RangeTest, FromCHARRANGE) {

    {
        CHARRANGE char_range;
        char_range.cpMin = 0;
        char_range.cpMax = -1;
        
        auto range = zaf::Range::FromCHARRANGE(char_range);
        ASSERT_EQ(range.index, 0);
        ASSERT_EQ(range.length, (std::numeric_limits<std::size_t>::max)());
    }

    {
        CHARRANGE char_range;
        char_range.cpMin = 20;
        char_range.cpMax = 30;

        auto range = zaf::Range::FromCHARRANGE(char_range);
        ASSERT_EQ(range.index, 20);
        ASSERT_EQ(range.length, 10);
    }
}


TEST(RangeTest, ToCHARRANGE) {

    {
        auto range = zaf::Range::Infinite();
        auto char_range = range.ToCHARRANGE();
        ASSERT_EQ(char_range.cpMin, 0);
        ASSERT_EQ(char_range.cpMax, -1);
    }

    {
        zaf::Range range{ 12, 44 };
        auto char_range = range.ToCHARRANGE();
        ASSERT_EQ(char_range.cpMin, 12);
        ASSERT_EQ(char_range.cpMax, 56);
    }
}


TEST(RangeTest, ToString) {

    zaf::Range range;
    ASSERT_EQ(range.ToString(), L"0,0");

    range.index = 2424;
    range.length = 778982;
    ASSERT_EQ(range.ToString(), L"2424,778982");
}


TEST(RangeTest, Comparison) {

    //Equal
    {
        zaf::Range r1;
        zaf::Range r2;
        ASSERT_TRUE(r1 == r2);

        r1 = zaf::Range{ 10, 10 };
        r2 = zaf::Range{ 10, 10 };
        ASSERT_TRUE(r1 == r2);
    }

    //Not equal
    {
        zaf::Range r1;
        zaf::Range r2{ 1, 2 };
        ASSERT_TRUE(r1 != r2);

        r1 = zaf::Range{ 8, 8 };
        r2 = zaf::Range{ 8, 7 };
        ASSERT_TRUE(r1 != r2);

        r1 = zaf::Range{ 5, 98 };
        r2 = zaf::Range{ 6, 98 };
        ASSERT_TRUE(r1 != r2);
    }

    //Less
    {
        zaf::Range r1{ 3, 3 };
        zaf::Range r2{ 4, 0};
        ASSERT_TRUE(r1 < r2);

        r1 = zaf::Range{ 3, 0 };
        r2 = zaf::Range{ 3, 1 };
        ASSERT_TRUE(r1 < r2);
    }
}


TEST(RangeTest, IsEqual) {

    zaf::Range r1;
    zaf::Range r2;
    ASSERT_TRUE(r1.IsEqual(r2));

    r1 = zaf::Range{ 0, 5 };
    r2 = zaf::Range{ 0, 5 };
    ASSERT_TRUE(r1.IsEqual(r2));

    r2 = zaf::Range{ 0, 4 };
    ASSERT_FALSE(r1.IsEqual(r2));
}


TEST(RangeTest, Hash) {

    zaf::Range r1;
    zaf::Range r2;
    ASSERT_EQ(r1.Hash(), r2.Hash());

    r1 = zaf::Range{ 78, 33 };
    r2 = zaf::Range{ 78, 33 };
    ASSERT_EQ(r1.Hash(), r2.Hash());
}