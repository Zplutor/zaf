#include <gtest/gtest.h>
#include <zaf/control/textual/styled_text.h>
#include <zaf/control/textual/styled_text_view.h>

using namespace zaf;
using namespace zaf::textual;

TEST(StyledTextViewTest, Construct) {

    {
        StyledText styled_text;
        ASSERT_NO_THROW(StyledTextView(styled_text, {}));
        ASSERT_NO_THROW(StyledTextView(styled_text, { 0, 1 }));
        ASSERT_THROW(StyledTextView(styled_text, Range{ 1, 0 }), PreconditionError);
    }

    {
        StyledText styled_text{ L"01234" };
        ASSERT_NO_THROW(StyledTextView(styled_text, {}));
        ASSERT_NO_THROW(StyledTextView(styled_text, { 0, 3 }));
        ASSERT_NO_THROW(StyledTextView(styled_text, { 0, 10 }));
        ASSERT_NO_THROW(StyledTextView(styled_text, { 3, 0 }));
        ASSERT_NO_THROW(StyledTextView(styled_text, { 3, 1 }));
        ASSERT_NO_THROW(StyledTextView(styled_text, { 4, 1 }));
        ASSERT_NO_THROW(StyledTextView(styled_text, { 5, 0 }));
        ASSERT_NO_THROW(StyledTextView(styled_text, { 5, 1 }));
        ASSERT_NO_THROW(StyledTextView(styled_text, { 5, 10 }));
        ASSERT_THROW(StyledTextView(styled_text, { 6, 0 }), PreconditionError);
        ASSERT_THROW(StyledTextView(styled_text, { 6, 1 }), PreconditionError);
        ASSERT_THROW(StyledTextView(styled_text, { 6, 10 }), PreconditionError);
    }
}


TEST(StyledTextViewTest, Text) {

    auto test = [](
        const std::wstring& original,
        const Range& range,
        const std::wstring& expected) {
    
        StyledText styled_text{ original };
        StyledTextView view{ styled_text, range };
        return view.Text() == expected;
    };

    ASSERT_TRUE(test(L"", {}, L""));
    ASSERT_TRUE(test(L"0123456789", {}, L""));
    ASSERT_TRUE(test(L"0123456789", { 0, 1 }, L"0"));
    ASSERT_TRUE(test(L"0123456789", { 3, 3 }, L"345"));
    ASSERT_TRUE(test(L"0123456789", { 7, 5 }, L"789"));
}