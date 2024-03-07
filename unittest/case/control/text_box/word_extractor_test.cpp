#include <gtest/gtest.h>
#include <zaf/control/textual/word_extractor.h>

using namespace zaf;
using namespace zaf::textual;

TEST(WordExtractorTest, Default) {

    auto extractor = DefaultWordExtractor();

    //Extract the whole text.
    {
        ASSERT_EQ(extractor(L"", 0), Range());
        ASSERT_EQ(extractor(L"1", 0), Range(0, 1));
        ASSERT_EQ(extractor(L"1", 1), Range(0, 1));
        ASSERT_EQ(extractor(L"1a", 0), Range(0, 2));
        ASSERT_EQ(extractor(L"1a", 1), Range(0, 2));
        ASSERT_EQ(extractor(L"1a", 2), Range(0, 2));
        ASSERT_EQ(extractor(L"1a2", 0), Range(0, 3));
        ASSERT_EQ(extractor(L"1a2", 1), Range(0, 3));
        ASSERT_EQ(extractor(L"1a2", 2), Range(0, 3));
        ASSERT_EQ(extractor(L"1a2", 3), Range(0, 3));
        ASSERT_EQ(extractor(L"1a2", 4), Range(0, 3));
    }

    {
        constexpr auto text = L"text.empty()";
        ASSERT_EQ(extractor(text, 0), Range(0, 4));
        ASSERT_EQ(extractor(text, 1), Range(0, 4));
        ASSERT_EQ(extractor(text, 2), Range(0, 4));
        ASSERT_EQ(extractor(text, 3), Range(0, 4));
        ASSERT_EQ(extractor(text, 4), Range(4, 1));
        ASSERT_EQ(extractor(text, 5), Range(5, 5));
        ASSERT_EQ(extractor(text, 6), Range(5, 5));
        ASSERT_EQ(extractor(text, 7), Range(5, 5));
        ASSERT_EQ(extractor(text, 8), Range(5, 5));
        ASSERT_EQ(extractor(text, 9), Range(5, 5));
        ASSERT_EQ(extractor(text, 10), Range(10, 1));
        ASSERT_EQ(extractor(text, 11), Range(11, 1));
        ASSERT_EQ(extractor(text, 12), Range(11, 1));
        ASSERT_EQ(extractor(text, 13), Range(11, 1));
    }
}