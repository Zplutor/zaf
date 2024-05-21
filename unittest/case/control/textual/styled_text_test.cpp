#include <gtest/gtest.h>
#include <zaf/control/textual/styled_text.h>
#include <zaf/creation.h>

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
        ASSERT_NO_THROW(styled_text.GetSubText(Range{ 0, 1 }));
    }

    //Non-empty styled text.
    {
        StyledText styled_text{ L"123" };
        ASSERT_THROW(styled_text.GetSubText(Range{ 4, 0 }), PreconditionError);
        ASSERT_NO_THROW(styled_text.GetSubText(Range{ 3, 2 }));
    }
}


TEST(StyledTextTest, GetSubText) {

    StyledText styled_text{ L"0123456789" };
    styled_text.SetDefaultFont(Font{ L"default-font" });
    styled_text.SetDefaultTextColor(Color::Red());
    styled_text.SetDefaultTextBackColor(Color::Green());

    styled_text.SetFontInRange(Font{ L"1" }, Range{ 0, 4 });
    styled_text.SetTextColorInRange(Color::Yellow(), Range{ 6, 4 });
    styled_text.SetTextBackColorInRange(Color::Gray(), Range{ 3, 4 });

    auto inline_object = Create<InlineObject>();
    styled_text.AttachInlineObjectToRange(inline_object, Range{ 4, 2 });

    {
        auto sub_text = styled_text.GetSubText(Range::FromIndexPair(2, 7));
        ASSERT_EQ(sub_text.Text(), L"23456");
        ASSERT_EQ(sub_text.DefaultFont(), styled_text.DefaultFont());

        auto text_color = sub_text.DefaultTextColorPicker().target<ConstantColorPicker>();
        ASSERT_TRUE(text_color);
        ASSERT_EQ(text_color->GetColor(), Color::Red());

        auto text_back_color = sub_text.DefaultTextBackColorPicker().target<ConstantColorPicker>();
        ASSERT_TRUE(text_back_color);
        ASSERT_EQ(text_back_color->GetColor(), Color::Green());

        //Ranged font
        ASSERT_FALSE(sub_text.RangedFonts().IsEmpty());
        const auto& font_item = *sub_text.RangedFonts().begin();
        ASSERT_EQ(font_item.Range(), Range(0, 2));
        ASSERT_EQ(font_item.Font().family_name, L"1");

        //Ranged text color picker
        ASSERT_FALSE(sub_text.RangedTextColorPicker().IsEmpty());
        const auto& text_color_item = *sub_text.RangedTextColorPicker().begin();
        ASSERT_EQ(text_color_item.Range(), Range(4, 1));
        ASSERT_EQ(
            text_color_item.ColorPicker().target<ConstantColorPicker>()->GetColor(),
            Color::Yellow());

        //Ranged text back color picker
        ASSERT_FALSE(sub_text.RangedTextBackColorPickers().IsEmpty());
        const auto& text_back_color_item = *sub_text.RangedTextBackColorPickers().begin();
        ASSERT_EQ(text_back_color_item.Range(), Range(1, 4));
        ASSERT_EQ(
            text_back_color_item.ColorPicker().target<ConstantColorPicker>()->GetColor(),
            Color::Gray());

        //Inline object
        ASSERT_FALSE(sub_text.InlineObjects().IsEmpty());
        ASSERT_EQ(sub_text.InlineObjects().begin()->Range(), Range(2, 2));
        //The object should be cloned.
        ASSERT_NE(sub_text.InlineObjects().begin()->Object(), inline_object);
    }
}