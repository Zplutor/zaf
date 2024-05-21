#include <gtest/gtest.h>
#include <zaf/control/textual/styled_text.h>

using namespace zaf;
using namespace zaf::textual;

static_assert(!std::is_copy_constructible_v<StyledText>);
static_assert(!std::is_copy_assignable_v<StyledText>);
static_assert(std::is_move_constructible_v<StyledText>);
static_assert(std::is_move_assignable_v<StyledText>);

TEST(StyledTextTest, SetTextInRange) {

    StyledText styled_text(L"StyledText");
    auto new_range = styled_text.SetTextInRange(L"ED-", Range{ 4, 2 });
    ASSERT_EQ(new_range, Range(4, 3));
    ASSERT_EQ(styled_text.Text(), L"StylED-Text");
}


TEST(StyledTextTest, AppendText) {

    StyledText styled_text(L"StyledText");
    auto new_range = styled_text.AppendText(L"-append");
    ASSERT_EQ(new_range, Range(10, 7));
    ASSERT_EQ(styled_text.Text(), L"StyledText-append");
}


TEST(StyledTextTest, GetSubTextPreconditionError) {

    //Empty styled text.
    {
        StyledText styled_text;
        ASSERT_THROW(styled_text.GetSubText(Range{ 1, 0 }), PreconditionError);
    }

    //Non-empty styled text.
}


TEST(StyledTextTest, GetSubTextEmptyText) {

    StyledText styled_text;
    styled_text.SetDefaultFont(Font{ L"default-font" });
    styled_text.SetDefaultTextColor(Color::Red());
    styled_text.SetDefaultTextBackColor(Color::Green());

    auto test = [&styled_text](const Range& sub_range) {
    
        auto sub_text = styled_text.GetSubText(sub_range);
        if (!sub_text.Text().empty()) {
            return false;
        }

        if (sub_text.DefaultFont().FamilyName() != L"default-font") {
            return false;
        }

        auto text_color = sub_text.DefaultTextColorPicker().target<ConstantColorPicker>();
        if (text_color->GetColor() != Color::Red()) {
            return false;
        }

        auto text_back_color = sub_text.DefaultTextBackColorPicker().target<ConstantColorPicker>();
        if (text_back_color->GetColor() != Color::Green()) {
            return false;
        }

        return true;
    };

    ASSERT_TRUE(test(Range(0, 0)));
    ASSERT_TRUE(test(Range(0, 2)));
}