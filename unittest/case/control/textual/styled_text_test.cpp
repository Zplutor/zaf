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
    ASSERT_TRUE(styled_text.RangedTextColors().IsEmpty());
    ASSERT_TRUE(styled_text.RangedTextBackColors().IsEmpty());
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


TEST(StyledTextTest, SetStyledTextInRange_Precondition) {

    StyledText slice;

    StyledText styled_text;
    ASSERT_THROW(styled_text.SetStyledTextInRange(slice, Range{ 1, 0 }), PreconditionError);
    ASSERT_THROW(styled_text.SetStyledTextInRange(slice, Range{ 1, 10 }), PreconditionError);
    ASSERT_NO_THROW(styled_text.SetStyledTextInRange(slice, Range{ 0, 0 }));
    ASSERT_NO_THROW(styled_text.SetStyledTextInRange(slice, Range{ 0, 10 }));
}


TEST(StyledTextTest, SetStyledTextInRange_Text) {

    auto test = [](
        const std::wstring& original_text,
        const std::wstring& slice_text,
        const Range& range,
        const std::wstring& expected_result,
        const Range& returned_range) {
    
        StyledText styled_text{ original_text };
        auto new_range = styled_text.SetStyledTextInRange(
            StyledText{ slice_text },
            range);
        return new_range == returned_range && styled_text.Text() == expected_result;
    };

    ASSERT_TRUE(test(L"", L"", Range(0, 0), L"", Range(0, 0)));
    ASSERT_TRUE(test(L"", L"", Range(0, 2), L"", Range(0, 0)));
    ASSERT_TRUE(test(L"", L"1", Range(0, 0), L"1", Range(0, 1)));
    ASSERT_TRUE(test(L"", L"12", Range(0, 10), L"12", Range(0, 2)));

    ASSERT_TRUE(test(L"01234", L"", Range(0, 0), L"01234", Range(0, 0)));
    ASSERT_TRUE(test(L"01234", L"", Range(0, 1), L"1234", Range(0, 0)));
    ASSERT_TRUE(test(L"01234", L"", Range(0, 10), L"", Range(0, 0)));
    ASSERT_TRUE(test(L"01234", L"", Range(2, 0), L"01234", Range(2, 0)));
    ASSERT_TRUE(test(L"01234", L"", Range(2, 5), L"01", Range(2, 0)));
    ASSERT_TRUE(test(L"01234", L"", Range(5, 0), L"01234", Range(5, 0)));
    ASSERT_TRUE(test(L"01234", L"", Range(5, 5), L"01234", Range(5, 0)));
    ASSERT_TRUE(test(L"01234", L"", Range(0, 50), L"", Range(0, 0)));
    ASSERT_TRUE(test(L"01234", L"A", Range(0, 0), L"A01234", Range(0, 1)));
    ASSERT_TRUE(test(L"01234", L"A", Range(2, 0), L"01A234", Range(2, 1)));
    ASSERT_TRUE(test(L"01234", L"AB", Range(5, 1), L"01234AB", Range(5, 2)));
    ASSERT_TRUE(test(L"01234", L"AB", Range(1, 3), L"0AB4", Range(1, 2)));
}


TEST(StyledTextTest, SetStyledTextInRange_DefaultFont) {

    //Ranged fonts won't change if the default fonts are equal.
    {
        StyledText slice{ L"ABC" };
        slice.SetDefaultFont(Font{ L"default" });

        StyledText styled_text{ L"012345" };
        styled_text.SetDefaultFont(Font{ L"default" });
        styled_text.SetStyledTextInRange(slice, Range{ 2, 2 });
        ASSERT_TRUE(styled_text.RangedFonts().IsEmpty());
    }

    //The default font of slice will be set as a ranged font in the styled text.
    {
        StyledText slice{ L"ABC" }; 
        slice.SetDefaultFont(Font{ L"slice" });

        StyledText styled_text{ L"012345" };
        styled_text.SetDefaultFont(Font{ L"default" });
        styled_text.SetStyledTextInRange(slice, Range{ 2, 2 });
        ASSERT_EQ(styled_text.RangedFonts().Count(), 1);
        ASSERT_EQ(styled_text.RangedFonts().begin()->Range(), Range(2, 3));
        ASSERT_EQ(styled_text.RangedFonts().begin()->Value().family_name, L"slice");
    }

    //The ranged font will override the default font of the slice.
    {
        StyledText slice{ L"ABCD" };
        slice.SetDefaultFont(Font{ L"slice" });
        slice.SetFontInRange(Font{ L"ranged" }, Range{ 2, 2 });

        //012345 -> 01ABCD45
        StyledText styled_text{ L"012345" };
        styled_text.SetDefaultFont(Font{ L"default" });
        styled_text.SetStyledTextInRange(slice, Range{ 2, 2 });
        ASSERT_EQ(styled_text.RangedFonts().Count(), 2);

        auto iterator = styled_text.RangedFonts().begin();
        ASSERT_EQ(iterator->Range(), Range(2, 2));
        ASSERT_EQ(iterator->Value().family_name, L"slice");

        ++iterator;
        ASSERT_EQ(iterator->Range(), Range(4, 2));
        ASSERT_EQ(iterator->Value().family_name, L"ranged");
    }
}


TEST(StyledTextTest, SetStyledTextInRange_RangedFonts) {

    {
        StyledText slice{ L"AABB" };
        slice.SetFontInRange(Font{ L"A" }, Range{ 0, 2 });
        slice.SetFontInRange(Font{ L"B" }, Range{ 2, 2 });

        // 012345 -> 012AABB5
        StyledText styled_text{ L"012345" };
        styled_text.SetStyledTextInRange(slice, Range{ 3, 2 });
        ASSERT_EQ(styled_text.RangedFonts().Count(), 2);

        auto iterator = styled_text.RangedFonts().begin();
        ASSERT_EQ(iterator->Range(), Range(3, 2));
        ASSERT_EQ(iterator->Value().family_name, L"A");

        ++iterator;
        ASSERT_EQ(iterator->Range(), Range(5, 2));
        ASSERT_EQ(iterator->Value().family_name, L"B");
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
        ASSERT_EQ(sub_text.DefaultTextColor(), Color::Red());
        ASSERT_EQ(sub_text.DefaultTextBackColor(), Color::Green());

        //Ranged font
        ASSERT_FALSE(sub_text.RangedFonts().IsEmpty());
        const auto& font_item = *sub_text.RangedFonts().begin();
        ASSERT_EQ(font_item.Range(), Range(0, 2));
        ASSERT_EQ(font_item.Value().family_name, L"1");

        //Ranged text color picker
        ASSERT_FALSE(sub_text.RangedTextColors().IsEmpty());
        const auto& text_color_item = *sub_text.RangedTextColors().begin();
        ASSERT_EQ(text_color_item.Range(), Range(4, 1));
        ASSERT_EQ(text_color_item.Value(), Color::Yellow());

        //Ranged text back color picker
        ASSERT_FALSE(sub_text.RangedTextBackColors().IsEmpty());
        const auto& text_back_color_item = *sub_text.RangedTextBackColors().begin();
        ASSERT_EQ(text_back_color_item.Range(), Range(1, 4));
        ASSERT_EQ(text_back_color_item.Value(), Color::Gray());

        //Inline object
        ASSERT_FALSE(sub_text.InlineObjects().IsEmpty());
        ASSERT_EQ(sub_text.InlineObjects().begin()->Range(), Range(2, 2));
        //The object should be cloned.
        ASSERT_NE(sub_text.InlineObjects().begin()->Object(), inline_object);
    }
}