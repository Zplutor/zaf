#include <gtest/gtest.h>
#include <zaf/internal/textual/line_utility.h>

using namespace zaf::internal;
using namespace zaf::textual;

TEST(LineUtilityTest, ReviseLinesInStyledText_NoMultiline) {

    auto test = [](const std::wstring& original_text, const std::wstring& expected) {
        StyledText styled_text(original_text);
        ReviseLinesInStyledText(styled_text, false, LineBreak::Unspecific, nullptr);
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


TEST(LineUtilityTest, ReviseLinesInStyledTextView_NoMultiline) {

    StyledText styled_text{ L"line" };
    auto revised_styled_text = ReviseLinesInStyledTextView(
        styled_text,
        false,
        LineBreak::Unspecific);

    ASSERT_TRUE(
        std::holds_alternative<std::reference_wrapper<const StyledText>>(revised_styled_text));
    ASSERT_EQ(
        std::get<std::reference_wrapper<const StyledText>>(revised_styled_text).get().Text(), 
        L"line");

    styled_text = StyledText(L"line1\r\nline2\r\nline3");
    revised_styled_text = ReviseLinesInStyledTextView(
        styled_text,
        false,
        LineBreak::Unspecific);
    ASSERT_TRUE(std::holds_alternative<StyledText>(revised_styled_text));
    ASSERT_EQ(std::get<StyledText>(revised_styled_text).Text(), L"line1");
}


TEST(LineUtilityTest, ReviseLinesInText_NoMultiline) {

    std::wstring text{ L"line1\r\nline2\r\nline3" };
    ReviseLinesInText(text, false, LineBreak::Unspecific);
    ASSERT_EQ(text, L"line1");
}


TEST(LineUtilityTest, ReviseLinesInTextView_NoMultiline) {

    auto revised_text = ReviseLinesInTextView(L"line", false, LineBreak::Unspecific);
    ASSERT_TRUE(std::holds_alternative<std::wstring_view>(revised_text));
    ASSERT_EQ(std::get<std::wstring_view>(revised_text), L"line");

    revised_text = ReviseLinesInTextView(
        L"line1\r\nline2\r\nline3",
        false,
        LineBreak::Unspecific);
    ASSERT_TRUE(std::holds_alternative<std::wstring_view>(revised_text));
    ASSERT_EQ(std::get<std::wstring_view>(revised_text), L"line1");
}


TEST(LineUtilityTest, ReviseLinesInStyledText_LineBreaks) {

    auto test = [](
        LineBreak line_break,
        const std::wstring& original_text, 
        const std::wstring& expected) {

        StyledText styled_text(original_text);
        ReviseLinesInStyledText( styled_text, true, line_break, nullptr);
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


TEST(LineUtilityTest, ReviseLinesInStyledTextView_LineBreaks) {

    auto test = [](
        LineBreak line_break,
        const std::wstring& original_text,
        bool is_view,
        const std::wstring& expected) {

        StyledText styled_text(original_text);
        auto revised = ReviseLinesInStyledTextView(
            styled_text,
            true,
            line_break);

        if (is_view != std::holds_alternative<std::reference_wrapper<const StyledText>>(revised)) {
            return false;
        }

        if (is_view) {
            return
                std::get<std::reference_wrapper<const StyledText>>(revised).get().Text() ==
                expected;
        }
        else {
            return std::get<StyledText>(revised).Text() == expected;
        }
    };

    ASSERT_TRUE(test(LineBreak::Unspecific, L"l1\r\nl2\rl3\nl4", true, L"l1\r\nl2\rl3\nl4"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"l1\r\nl2\rl3\nl4", false, L"l1\r\nl2\r\nl3\r\nl4"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"l1\r\nl2\r\nl3", true, L"l1\r\nl2\r\nl3"));
    ASSERT_TRUE(test(LineBreak::CR, L"l1\r\nl2\rl3\nl4", false, L"l1\rl2\rl3\rl4"));
    ASSERT_TRUE(test(LineBreak::CR, L"l1\rl2\rl3", true, L"l1\rl2\rl3"));
    ASSERT_TRUE(test(LineBreak::LF, L"l1\r\nl2\rl3\nl4", false, L"l1\nl2\nl3\nl4"));
    ASSERT_TRUE(test(LineBreak::LF, L"l1\nl2\nl3", true, L"l1\nl2\nl3"));
}


TEST(LineUtilityTest, ReviseLinesInText_LineBreaks) {

    auto test = [](
        LineBreak line_break,
        const std::wstring& original_text,
        const std::wstring& expected) {

        std::wstring text(original_text);
        ReviseLinesInText(text, true, line_break);
        return text == expected;
    };

    ASSERT_TRUE(test(LineBreak::Unspecific, L"l1\r\nl2\rl3\nl4", L"l1\r\nl2\rl3\nl4"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"l1\r\nl2\rl3\nl4", L"l1\r\nl2\r\nl3\r\nl4"));
    ASSERT_TRUE(test(LineBreak::CR, L"l1\r\nl2\rl3\nl4", L"l1\rl2\rl3\rl4"));
    ASSERT_TRUE(test(LineBreak::LF, L"l1\r\nl2\rl3\nl4", L"l1\nl2\nl3\nl4"));
}


TEST(LineUtilityTest, ReviseLinesInTextView_LineBreaks) {

    auto test = [](
        LineBreak line_break,
        const std::wstring& original_text,
        bool is_string_view,
        const std::wstring& expected) {

        auto revised_text = ReviseLinesInTextView(original_text, true, line_break);
        if (is_string_view != std::holds_alternative<std::wstring_view>(revised_text)) {
            return false;
        }

        if (is_string_view) {
            return std::get<std::wstring_view>(revised_text) == expected;
        }
        else {
            return std::get<std::wstring>(revised_text) == expected;
        }
    };

    ASSERT_TRUE(test(LineBreak::Unspecific, L"l1\r\nl2\rl3\nl4", true, L"l1\r\nl2\rl3\nl4"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"l1\r\nl2\rl3\nl4", false, L"l1\r\nl2\r\nl3\r\nl4"));
    ASSERT_TRUE(test(LineBreak::CRLF, L"l1\r\nl2\r\nl3", true, L"l1\r\nl2\r\nl3"));
    ASSERT_TRUE(test(LineBreak::CR, L"l1\r\nl2\rl3\nl4", false, L"l1\rl2\rl3\rl4"));
    ASSERT_TRUE(test(LineBreak::CR, L"l1\rl2\rl3", true, L"l1\rl2\rl3"));
    ASSERT_TRUE(test(LineBreak::LF, L"l1\r\nl2\rl3\nl4", false, L"l1\nl2\nl3\nl4"));
    ASSERT_TRUE(test(LineBreak::LF, L"l1\nl2\nl3", true, L"l1\nl2\nl3"));
}