#include <gtest/gtest.h>
#include <zaf/internal/textual/line_utility.h>

using namespace zaf::internal;
using namespace zaf::textual;

TEST(LineUtilityTest, ReviseLinesInStyledText_NoMultiline) {

    auto test = [](const std::wstring& original_text, const std::wstring& expected) {
        StyledText styled_text(original_text);
        LineUtility::ReviseLinesInStyledText(styled_text, false, LineBreak::Unspecific);
        return styled_text.Text() == expected;
    };

    ASSERT_TRUE(test(L"", L""));
    ASSERT_TRUE(test(L"aaaaaa", L"aaaaaa"));
    ASSERT_TRUE(test(L"\r\n", L""));
    ASSERT_TRUE(test(L"\r", L""));
    ASSERT_TRUE(test(L"\n", L""));
    ASSERT_TRUE(test(L"line1\r\nline2\r\nline3", L"line1"));
    ASSERT_TRUE(test(L"line1\rline2\rline3", L"line1"));
    ASSERT_TRUE(test(L"line1\nline2\nline3", L"line1"));
}


TEST(LineUtilityTest, ReviseLinesInText_NoMultiline) {

    std::wstring text{ L"line1\r\nline2\r\nline3" };
    LineUtility::ReviseLinesInText(text, false, LineBreak::Unspecific);
    ASSERT_EQ(text, L"line1");
}


TEST(LineUtilityTest, ReviseLinesInStyledText_LineBreaks) {

    auto test = [](
        LineBreak line_break,
        const std::wstring& original_text, 
        const std::wstring& expected) {

        StyledText styled_text(original_text);
        LineUtility::ReviseLinesInStyledText( styled_text, true, line_break);
        return styled_text.Text() == expected;
    };

    ASSERT_TRUE(test(LineBreak::Unspecific, L"", L""));
    ASSERT_TRUE(test(LineBreak::Unspecific, L"line", L"line"));
    ASSERT_TRUE(test(LineBreak::Unspecific, L"l1\r\nl2\rl3\nl4", L"l1\r\nl2\rl3\nl4"));

    ASSERT_TRUE(test(LineBreak::CRLF, L"", L""));
    ASSERT_TRUE(test(LineBreak::CRLF, L"line", L"line"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"l1\r\nl2\rl3\nl4", L"l1\r\nl2\r\nl3\r\nl4"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"\r\n", L"\r\n"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"\r", L"\r\n"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"\n", L"\r\n"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"\r\n\r\n", L"\r\n\r\n"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"\r\r", L"\r\n\r\n"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"\n\n", L"\r\n\r\n"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"\r\n\r\r\n\n", L"\r\n\r\n\r\n\r\n"));

    ASSERT_TRUE(test(LineBreak::CR, L"", L""));
    ASSERT_TRUE(test(LineBreak::CR, L"line", L"line"));
    ASSERT_TRUE(test(LineBreak::CR, L"l1\r\nl2\rl3\nl4", L"l1\rl2\rl3\rl4"));
    ASSERT_TRUE(test(LineBreak::CR, L"\r\n", L"\r"));
    ASSERT_TRUE(test(LineBreak::CR, L"\r", L"\r"));
    ASSERT_TRUE(test(LineBreak::CR, L"\n", L"\r"));
    ASSERT_TRUE(test(LineBreak::CR, L"\r\n\r\n", L"\r\r"));
    ASSERT_TRUE(test(LineBreak::CR, L"\r\r", L"\r\r"));
    ASSERT_TRUE(test(LineBreak::CR, L"\n\n", L"\r\r"));
    ASSERT_TRUE(test(LineBreak::CR, L"\r\n\r\r\n\n", L"\r\r\r\r"));

    ASSERT_TRUE(test(LineBreak::LF, L"", L""));
    ASSERT_TRUE(test(LineBreak::LF, L"line", L"line"));
    ASSERT_TRUE(test(LineBreak::LF, L"l1\r\nl2\rl3\nl4", L"l1\nl2\nl3\nl4"));
    ASSERT_TRUE(test(LineBreak::LF, L"\r\n", L"\n"));
    ASSERT_TRUE(test(LineBreak::LF, L"\r", L"\n"));
    ASSERT_TRUE(test(LineBreak::LF, L"\n", L"\n"));
    ASSERT_TRUE(test(LineBreak::LF, L"\r\n\r\n", L"\n\n"));
    ASSERT_TRUE(test(LineBreak::LF, L"\r\r", L"\n\n"));
    ASSERT_TRUE(test(LineBreak::LF, L"\n\n", L"\n\n"));
    ASSERT_TRUE(test(LineBreak::LF, L"\r\n\r\r\n\n", L"\n\n\n\n"));
}


TEST(LineUtilityTest, ReviseLinesInText_LineBreaks) {

    auto test = [](
        LineBreak line_break,
        const std::wstring& original_text,
        const std::wstring& expected) {

        std::wstring text(original_text);
        LineUtility::ReviseLinesInText(text, true, line_break);
        return text == expected;
    };

    ASSERT_TRUE(test(LineBreak::Unspecific, L"l1\r\nl2\rl3\nl4", L"l1\r\nl2\rl3\nl4"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"l1\r\nl2\rl3\nl4", L"l1\r\nl2\r\nl3\r\nl4"));
    ASSERT_TRUE(test(LineBreak::CR, L"l1\r\nl2\rl3\nl4", L"l1\rl2\rl3\rl4"));
    ASSERT_TRUE(test(LineBreak::LF, L"l1\r\nl2\rl3\nl4", L"l1\nl2\nl3\nl4"));
}