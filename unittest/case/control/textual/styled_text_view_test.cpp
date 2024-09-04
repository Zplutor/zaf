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


TEST(StyledTextViewTest, GetFontAtIndex) {

    StyledText styled_text{ L"0123456789" };
    styled_text.SetDefaultFont(Font{ L"default" });
    styled_text.SetFontInRange(Font{ L"font1" }, Range{ 1, 2 });
    styled_text.SetFontInRange(Font{ L"font2" }, Range{ 5, 3 });

    {
        StyledTextView view{ styled_text, { 0, 10 } };
        ASSERT_EQ(view.GetFontAtIndex(0).family_name, L"default");
        ASSERT_EQ(view.GetFontAtIndex(1).family_name, L"font1");
        ASSERT_EQ(view.GetFontAtIndex(2).family_name, L"font1");
        ASSERT_EQ(view.GetFontAtIndex(3).family_name, L"default");
        ASSERT_EQ(view.GetFontAtIndex(4).family_name, L"default");
        ASSERT_EQ(view.GetFontAtIndex(5).family_name, L"font2");
        ASSERT_EQ(view.GetFontAtIndex(6).family_name, L"font2");
        ASSERT_EQ(view.GetFontAtIndex(7).family_name, L"font2");
        ASSERT_EQ(view.GetFontAtIndex(8).family_name, L"default");
        ASSERT_EQ(view.GetFontAtIndex(9).family_name, L"default");
        ASSERT_EQ(view.GetFontAtIndex(10).family_name, L"default");
    }

    {
        StyledTextView view{ styled_text, { 1, 3 } };
        ASSERT_EQ(view.GetFontAtIndex(0).family_name, L"font1");
        ASSERT_EQ(view.GetFontAtIndex(1).family_name, L"font1");
        ASSERT_EQ(view.GetFontAtIndex(2).family_name, L"default");
        ASSERT_EQ(view.GetFontAtIndex(3).family_name, L"default");
    }

    {
        StyledTextView view{ styled_text, { 4, 3 } };
        ASSERT_EQ(view.GetFontAtIndex(0).family_name, L"default");
        ASSERT_EQ(view.GetFontAtIndex(1).family_name, L"font2");
        ASSERT_EQ(view.GetFontAtIndex(2).family_name, L"font2");
        //The font should be default at the end of the view, even if a font is set in the original
        //styled text at where the view ends.
        ASSERT_EQ(view.GetFontAtIndex(3).family_name, L"default");
    }
}


TEST(StyledTextViewTest, GetTextColorAtIndex) {

    StyledText styled_text{ L"0123456789" };
    styled_text.SetDefaultTextColor(Color::Lime());
    styled_text.SetTextColorInRange(Color::Green(), Range{ 1, 2 });
    styled_text.SetTextColorInRange(Color::Blue(), Range{ 5, 3 });

    {
        StyledTextView view{ styled_text, { 0, 10 } };
        ASSERT_EQ(view.GetTextColorAtIndex(0), Color::Lime());
        ASSERT_EQ(view.GetTextColorAtIndex(1), Color::Green());
        ASSERT_EQ(view.GetTextColorAtIndex(2), Color::Green());
        ASSERT_EQ(view.GetTextColorAtIndex(3), Color::Lime());
        ASSERT_EQ(view.GetTextColorAtIndex(4), Color::Lime());
        ASSERT_EQ(view.GetTextColorAtIndex(5), Color::Blue());
        ASSERT_EQ(view.GetTextColorAtIndex(6), Color::Blue());
        ASSERT_EQ(view.GetTextColorAtIndex(7), Color::Blue());
        ASSERT_EQ(view.GetTextColorAtIndex(8), Color::Lime());
        ASSERT_EQ(view.GetTextColorAtIndex(9), Color::Lime());
        ASSERT_EQ(view.GetTextColorAtIndex(10), Color::Lime());
    }

    {
        StyledTextView view{ styled_text, { 1, 3 } };
        ASSERT_EQ(view.GetTextColorAtIndex(0), Color::Green());
        ASSERT_EQ(view.GetTextColorAtIndex(1), Color::Green());
        ASSERT_EQ(view.GetTextColorAtIndex(2), Color::Lime());
        ASSERT_EQ(view.GetTextColorAtIndex(3), Color::Lime());
    }

    {
        StyledTextView view{ styled_text, { 4, 3 } };
        ASSERT_EQ(view.GetTextColorAtIndex(0), Color::Lime());
        ASSERT_EQ(view.GetTextColorAtIndex(1), Color::Blue());
        ASSERT_EQ(view.GetTextColorAtIndex(2), Color::Blue());
        ASSERT_EQ(view.GetTextColorAtIndex(3), Color::Lime());
    }
}


TEST(StyledTextViewTest, GetTextBackColorAtIndex) {

    StyledText styled_text{ L"0123456789" };
    styled_text.SetDefaultTextBackColor(Color::Lime());
    styled_text.SetTextBackColorInRange(Color::Green(), Range{ 1, 2 });
    styled_text.SetTextBackColorInRange(Color::Blue(), Range{ 5, 3 });

    {
        StyledTextView view{ styled_text, { 0, 10 } };
        ASSERT_EQ(view.GetTextBackColorAtIndex(0), Color::Lime());
        ASSERT_EQ(view.GetTextBackColorAtIndex(1), Color::Green());
        ASSERT_EQ(view.GetTextBackColorAtIndex(2), Color::Green());
        ASSERT_EQ(view.GetTextBackColorAtIndex(3), Color::Lime());
        ASSERT_EQ(view.GetTextBackColorAtIndex(4), Color::Lime());
        ASSERT_EQ(view.GetTextBackColorAtIndex(5), Color::Blue());
        ASSERT_EQ(view.GetTextBackColorAtIndex(6), Color::Blue());
        ASSERT_EQ(view.GetTextBackColorAtIndex(7), Color::Blue());
        ASSERT_EQ(view.GetTextBackColorAtIndex(8), Color::Lime());
        ASSERT_EQ(view.GetTextBackColorAtIndex(9), Color::Lime());
        ASSERT_EQ(view.GetTextBackColorAtIndex(10), Color::Lime());
    }

    {
        StyledTextView view{ styled_text, { 1, 3 } };
        ASSERT_EQ(view.GetTextBackColorAtIndex(0), Color::Green());
        ASSERT_EQ(view.GetTextBackColorAtIndex(1), Color::Green());
        ASSERT_EQ(view.GetTextBackColorAtIndex(2), Color::Lime());
        ASSERT_EQ(view.GetTextBackColorAtIndex(3), Color::Lime());
    }

    {
        StyledTextView view{ styled_text, { 4, 3 } };
        ASSERT_EQ(view.GetTextBackColorAtIndex(0), Color::Lime());
        ASSERT_EQ(view.GetTextBackColorAtIndex(1), Color::Blue());
        ASSERT_EQ(view.GetTextBackColorAtIndex(2), Color::Blue());
        ASSERT_EQ(view.GetTextBackColorAtIndex(3), Color::Lime());
    }
}


TEST(StyledTextViewTest, GetInlineObjectAtIndex) {

    StyledText styled_text{ L"012345" };

    auto object1 = std::make_shared<InlineObject>();
    styled_text.AttachInlineObjectToRange(object1, Range{ 1, 1 });

    auto object2 = std::make_shared<InlineObject>();
    styled_text.AttachInlineObjectToRange(object2, Range{ 3, 2 });

    {
        StyledTextView view{ styled_text, { 0, 10 } };
        ASSERT_EQ(view.GetInlineObjectAtIndex(0), nullptr);
        ASSERT_EQ(view.GetInlineObjectAtIndex(1), object1);
        ASSERT_EQ(view.GetInlineObjectAtIndex(2), nullptr);
        ASSERT_EQ(view.GetInlineObjectAtIndex(3), object2);
        ASSERT_EQ(view.GetInlineObjectAtIndex(4), object2);
        ASSERT_EQ(view.GetInlineObjectAtIndex(5), nullptr);
        ASSERT_EQ(view.GetInlineObjectAtIndex(6), nullptr);
    }

    {
        StyledTextView view{ styled_text, { 1, 3 } };
        ASSERT_EQ(view.GetInlineObjectAtIndex(0), object1);
        ASSERT_EQ(view.GetInlineObjectAtIndex(1), nullptr);
        //The inline object can be seen in a view only if its range is fully contained by the view.
        ASSERT_EQ(view.GetInlineObjectAtIndex(2), nullptr); 
        ASSERT_EQ(view.GetInlineObjectAtIndex(3), nullptr);
    }
}


TEST(StyledTextViewTest, RangedFonts) {

    StyledText styled_text{ L"0123456789" };
    styled_text.SetDefaultFont(Font{ L"default" });
    styled_text.SetFontInRange(Font{ L"font1" }, Range{ 1, 2 });
    styled_text.SetFontInRange(Font{ L"font2" }, Range{ 5, 3 });

    {
        StyledTextView view{ styled_text, Range{ 3, 2 } };
        auto ranged_fonts = view.RangedFonts();
        ASSERT_TRUE(ranged_fonts.IsEmpty());
        ASSERT_EQ(ranged_fonts.Count(), 0);
    }

    {
        StyledTextView view{ styled_text, Range{ 5, 0 } };
        auto ranged_fonts = view.RangedFonts();
        ASSERT_TRUE(ranged_fonts.IsEmpty());
        ASSERT_EQ(ranged_fonts.Count(), 0);
    }

    {
        StyledTextView view{ styled_text, { 0, 10 } };
        auto ranged_fonts = view.RangedFonts();
        ASSERT_FALSE(ranged_fonts.IsEmpty());
        ASSERT_EQ(ranged_fonts.Count(), 2);

        auto iterator = ranged_fonts.begin();
        ASSERT_EQ((*iterator).Range(), Range(1, 2));
        ASSERT_EQ((*iterator).Value().family_name, L"font1");

        ++iterator;
        ASSERT_EQ((*iterator).Range(), Range(5, 3 ));
        ASSERT_EQ((*iterator).Value().family_name, L"font2");

        ++iterator;
        ASSERT_EQ(iterator, ranged_fonts.end());
    }

    {
        StyledTextView view{ styled_text, { 1, 3 } };
        auto ranged_fonts = view.RangedFonts();
        ASSERT_FALSE(ranged_fonts.IsEmpty());
        ASSERT_EQ(ranged_fonts.Count(), 1);

        auto iterator = ranged_fonts.begin();
        ASSERT_EQ((*iterator).Range(), Range(0, 2));
        ASSERT_EQ((*iterator).Value().family_name, L"font1");

        ++iterator;
        ASSERT_EQ(iterator, ranged_fonts.end());
    }

    {
        StyledTextView view{ styled_text, { 4, 3 } };
        auto ranged_fonts = view.RangedFonts();
        ASSERT_FALSE(ranged_fonts.IsEmpty());
        ASSERT_EQ(ranged_fonts.Count(), 1);

        auto iterator = ranged_fonts.begin();
        ASSERT_EQ(iterator->Range(), Range(1, 2));
        ASSERT_EQ(iterator->Value().family_name, L"font2");

        ++iterator;
        ASSERT_EQ(iterator, ranged_fonts.end());
    }
}


TEST(StyledTextViewTest, InlineObjects) {

    StyledText styled_text{ L"012345" };

    auto object1 = std::make_shared<InlineObject>();
    styled_text.AttachInlineObjectToRange(object1, Range{ 1, 1 });

    auto object2 = std::make_shared<InlineObject>();
    styled_text.AttachInlineObjectToRange(object2, Range{ 3, 2 });

    {
        StyledTextView view{ styled_text, {} };
        auto objects = view.InlineObjects();
        ASSERT_TRUE(objects.IsEmpty());
        ASSERT_EQ(objects.Count(), 0);
    }

    {
        StyledTextView view{ styled_text, { 0, 10} };
        auto objects = view.InlineObjects();
        ASSERT_FALSE(objects.IsEmpty());
        ASSERT_EQ(objects.Count(), 2);

        auto iterator = objects.begin();
        ASSERT_EQ(iterator->Range(), Range(1, 1));
        ASSERT_EQ(iterator->Object(), object1);

        ++iterator;
        ASSERT_EQ(iterator->Range(), Range(3, 2));
        ASSERT_EQ(iterator->Object(), object2);

        ++iterator;
        ASSERT_EQ(iterator, objects.end());
    }

    {
        StyledTextView view{ styled_text, { 1, 3 } };
        auto objects = view.InlineObjects();
        ASSERT_FALSE(objects.IsEmpty());
        ASSERT_EQ(objects.Count(), 1);

        auto iterator = objects.begin();
        ASSERT_EQ(iterator->Range(), Range(0, 1));
        ASSERT_EQ(iterator->Object(), object1);

        ++iterator;
        ASSERT_EQ(iterator, objects.end());
    }
}