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

    StyledText styled_text(L"Styled-Text");
    styled_text.SetFontInRange(Font{ L"font" }, Range{ 4, 3 });
    styled_text.SetTextColorInRange(Color::Red(), Range{ 4, 3 });
    styled_text.SetTextBackColorInRange(Color::Black(), Range{ 4, 3 });
    styled_text.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 6, 1 });

    auto new_range = styled_text.SetTextInRange(L"ED--", Range{ 4, 3 });
    ASSERT_EQ(new_range, Range(4, 4));
    ASSERT_EQ(styled_text.Text(), L"StylED--Text");

    ASSERT_TRUE(styled_text.RangedFonts().IsEmpty());
    ASSERT_TRUE(styled_text.RangedTextColorPickers().IsEmpty());
    ASSERT_TRUE(styled_text.RangedTextBackColorPickers().IsEmpty());
    ASSERT_TRUE(styled_text.InlineObjects().IsEmpty());

    //Throw exception if the index of the range exceeds the length of the text.
    ASSERT_THROW(styled_text.SetTextInRange(L"tail", Range{ 13, 0 }), PreconditionError);

    //Insert text at the end.
    new_range = styled_text.SetTextInRange(L"tail", Range{ 12, 10 });
    ASSERT_EQ(new_range, Range(12, 4));
    ASSERT_EQ(styled_text.Text(), L"StylED--Texttail");
}


TEST(StyledTextTest, AppendText) {

    StyledText styled_text(L"StyledText");
    auto new_range = styled_text.AppendText(L"-append");
    ASSERT_EQ(new_range, Range(10, 7));
    ASSERT_EQ(styled_text.Text(), L"StyledText-append");
}


TEST(StyledTextTest, SlicePrecondition) {

    //Empty styled text.
    {
        StyledText styled_text;
        ASSERT_THROW(styled_text.Slice(Range{ 1, 0 }), PreconditionError);
        ASSERT_NO_THROW(styled_text.Slice(Range{ 0, 1 }));
    }

    //Non-empty styled text.
    {
        StyledText styled_text{ L"123" };
        ASSERT_THROW(styled_text.Slice(Range{ 4, 0 }), PreconditionError);
        ASSERT_NO_THROW(styled_text.Slice(Range{ 3, 2 }));
    }
}


TEST(StyledTextTest, Slice) {

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
        auto slice = styled_text.Slice(Range::FromIndexPair(2, 7));
        ASSERT_EQ(slice.Index(), 2);
        ASSERT_EQ(slice.Text(), L"23456");
        ASSERT_EQ(slice.DefaultStyle().Font(), styled_text.DefaultFont());

        auto text_color = slice.DefaultStyle().TextColorPicker().target<ConstantColorPicker>();
        ASSERT_TRUE(text_color);
        ASSERT_EQ(text_color->GetColor(), Color::Red());

        auto text_back_color = 
            slice.DefaultStyle().TextBackColorPicker().target<ConstantColorPicker>();
        ASSERT_TRUE(text_back_color);
        ASSERT_EQ(text_back_color->GetColor(), Color::Green());

        //Ranged font
        ASSERT_FALSE(slice.RangedStyle().Fonts().IsEmpty());
        const auto& font_item = *slice.RangedStyle().Fonts().begin();
        ASSERT_EQ(font_item.Range(), Range(0, 4));
        ASSERT_EQ(font_item.Font().family_name, L"1");

        //Ranged text color picker
        ASSERT_FALSE(slice.RangedStyle().TextColorPickers().IsEmpty());
        const auto& text_color_item = *slice.RangedStyle().TextColorPickers().begin();
        ASSERT_EQ(text_color_item.Range(), Range(6, 4));
        ASSERT_EQ(
            text_color_item.ColorPicker().target<ConstantColorPicker>()->GetColor(),
            Color::Yellow());

        //Ranged text back color picker
        ASSERT_FALSE(slice.RangedStyle().TextBackColorPickers().IsEmpty());
        const auto& text_back_color_item = *slice.RangedStyle().TextBackColorPickers().begin();
        ASSERT_EQ(text_back_color_item.Range(), Range(3, 4));
        ASSERT_EQ(
            text_back_color_item.ColorPicker().target<ConstantColorPicker>()->GetColor(),
            Color::Gray());

        //Inline object
        ASSERT_FALSE(slice.RangedStyle().InlineObjects().IsEmpty());
        ASSERT_EQ(slice.RangedStyle().InlineObjects().begin()->Range(), Range(4, 2));
        //The object should be cloned.
        ASSERT_NE(slice.RangedStyle().InlineObjects().begin()->Object(), inline_object);
    }
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
        ASSERT_FALSE(sub_text.RangedTextColorPickers().IsEmpty());
        const auto& text_color_item = *sub_text.RangedTextColorPickers().begin();
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