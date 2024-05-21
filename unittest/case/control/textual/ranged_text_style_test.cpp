#include <gtest/gtest.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/textual/ranged_text_style.h>
#include <zaf/creation.h>

using namespace zaf;
using namespace zaf::internal;
using namespace zaf::textual;

static_assert(!std::is_copy_constructible_v<RangedTextStyle>);
static_assert(!std::is_copy_assignable_v<RangedTextStyle>);
static_assert(std::is_move_constructible_v<RangedTextStyle>);
static_assert(std::is_move_assignable_v<RangedTextStyle>);

static_assert(!std::is_copy_constructible_v<RangedFontItem>);
static_assert(!std::is_copy_assignable_v<RangedFontItem>);
static_assert(!std::is_move_constructible_v<RangedFontItem>);
static_assert(!std::is_move_assignable_v<RangedFontItem>);

TEST(RangedTextStyleTest, RangedTypesCheck) {

    static_assert(std::is_same_v<RangedTextStyle::FontAccessor::value_type, RangedFontItem>);

    static_assert(std::is_same_v<
        RangedTextStyle::FontAccessor::iterator::value_type,
        RangedFontItem>);

    //iterator
    static_assert(std::is_same_v<
        RangedTextStyle::FontAccessor::iterator::pointer, 
        RangedFontItem*>);

    static_assert(std::is_same_v<
        RangedTextStyle::FontAccessor::iterator::reference,
        RangedFontItem&>);

    //const_iterator
    static_assert(std::is_same_v<
        RangedTextStyle::FontAccessor::const_iterator::value_type,
        RangedFontItem>);

    static_assert(std::is_same_v<
        RangedTextStyle::FontAccessor::const_iterator::pointer,
        const RangedFontItem*>);

    static_assert(std::is_same_v<
        RangedTextStyle::FontAccessor::const_iterator::reference,
        const RangedFontItem&>);

    {
        RangedTextStyle ranged_style;
        auto iterator = ranged_style.Fonts().begin();
        static_assert(std::is_same_v<decltype(*iterator), RangedFontItem&>);
    }

    {
        RangedTextStyle ranged_style;
        auto iterator = ranged_style.Fonts().cbegin();
        static_assert(std::is_same_v<decltype(*iterator), const RangedFontItem&>);
    }

    {
        const RangedTextStyle ranged_style;
        auto iterator = ranged_style.Fonts().begin();
        static_assert(std::is_same_v<decltype(*iterator), const RangedFontItem&>);
    }
}


TEST(RangedTextStyleTest, Move) {

    RangedTextStyle origin;
    origin.SetFontInRange(Font{ L"move" }, Range{ 2, 2 });
    origin.SetTextColorPickerInRange(CreateColorPicker(Color::Green()), Range{ 3, 5 });
    origin.SetTextBackColorPickerInRange(CreateColorPicker(Color::Red()), Range{ 1, 4 });
    origin.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 10, 1 });

    const auto check_if_style_empty = [](const RangedTextStyle& style) {
        return 
            style.Fonts().IsEmpty() &&
            style.TextColorPickers().IsEmpty() &&
            style.TextBackColorPickers().IsEmpty() &&
            style.InlineObjects().IsEmpty();
    };

    const auto check_if_style_not_empty = [](const RangedTextStyle& style) {
        if (style.Fonts().IsEmpty() || 
            style.TextColorPickers().IsEmpty() ||
            style.TextBackColorPickers().IsEmpty() ||
            style.InlineObjects().IsEmpty()) {
            return false;
        }

        if (style.Fonts().begin()->Range() != Range{ 2, 2 } ||
            style.Fonts().begin()->Font().family_name != L"move") {
            return false;
        }

        if (style.TextColorPickers().begin()->Range() != Range{ 3, 5 }) {
            return false;
        }

        if (style.TextBackColorPickers().begin()->Range() != Range{ 1, 4 }) {
            return false;
        }

        if (style.InlineObjects().begin()->Range() != Range{ 10, 1 } ||
            style.InlineObjects().begin()->Object() == nullptr) {
            return false;
        }

        return true;
    };

    RangedTextStyle move_constructed = std::move(origin);
    ASSERT_TRUE(check_if_style_empty(origin));
    ASSERT_TRUE(check_if_style_not_empty(move_constructed));
    
    RangedTextStyle move_assigned;
    move_assigned = std::move(move_constructed);
    ASSERT_TRUE(check_if_style_empty(move_constructed));
    ASSERT_TRUE(check_if_style_not_empty(move_assigned));
}


TEST(RangedTextStyleTest, EnumerateFonts) {

    RangedTextStyle ranged_style;
    ranged_style.SetFontInRange(Font{ L"aa" }, Range{ 0, 1 });
    ranged_style.SetFontInRange(Font{ L"bb" }, Range{ 5, 2 });
    ranged_style.SetFontInRange(Font{ L"cc" }, Range{ 10, 3 });

    const std::vector<Range> expected_ranges{
        Range{ 0, 1 },
        Range{ 5, 2 },
        Range{ 10, 3 },
    };

    const std::vector<std::wstring> expected_strings{
        L"aa",
        L"bb",
        L"cc",
    };

    //Use range-for.
    {
        std::vector<Range> ranges;
        std::vector<std::wstring> strings;
        for (const auto& each_item : ranged_style.Fonts()) {
            ranges.push_back(each_item.Range());
            strings.push_back(each_item.Font().family_name);
        }
        ASSERT_EQ(ranges, expected_ranges);
        ASSERT_EQ(strings, expected_strings);
    }

    //Use iterator.
    {
        std::vector<Range> ranges;
        std::vector<std::wstring> strings;

        for (auto iterator = ranged_style.Fonts().begin();
             iterator != ranged_style.Fonts().end();
             ++iterator) {

            ranges.push_back(iterator->Range());
            strings.push_back(iterator->Font().family_name);
        }

        ASSERT_EQ(ranges, expected_ranges);
        ASSERT_EQ(strings, expected_strings);
    }

    //Use const iterator.
    {
        std::vector<Range> ranges;
        std::vector<std::wstring> strings;

        for (auto iterator = ranged_style.Fonts().cbegin();
            iterator != ranged_style.Fonts().cend();
            ++iterator) {

            ranges.push_back(iterator->Range());
            strings.push_back(iterator->Font().family_name);
        }

        ASSERT_EQ(ranges, expected_ranges);
        ASSERT_EQ(strings, expected_strings);
    }
}


TEST(RangedTextStyleTest, ModifyFontsInEnumeration) {

    RangedTextStyle ranged_style;
    ranged_style.SetFontInRange(Font{ L"aa" }, Range{ 0, 1 });
    ranged_style.SetFontInRange(Font{ L"bb" }, Range{ 5, 2 });
    ranged_style.SetFontInRange(Font{ L"cc" }, Range{ 10, 3 });

    for (auto& each_item : ranged_style.Fonts()) {
        each_item.Font().family_name += L"!!";
    }

    std::vector<std::wstring> strings;
    for (const auto& each_item : ranged_style.Fonts()) {
        strings.push_back(each_item.Font().family_name);
    }

    std::vector<std::wstring> expected_strings{
        L"aa!!",
        L"bb!!",
        L"cc!!",
    };
    ASSERT_EQ(strings, expected_strings);
}


TEST(RangedTextStyleText, FindItemContainsIndex) {

    RangedTextStyle ranged_style;
    ranged_style.SetFontInRange(Font{ L"aaaaa" }, Range{ 2, 2 });

    //Mutable ranged style.
    {
        auto& fonts = ranged_style.Fonts();
        ASSERT_EQ(fonts.FindItemContainsIndex(1), fonts.end());

        auto iterator = fonts.FindItemContainsIndex(2);
        ASSERT_NE(iterator, fonts.end());
        ASSERT_EQ(iterator->Range(), Range(2, 2));
        ASSERT_EQ(iterator->Font().family_name, L"aaaaa");

        iterator->Font().family_name = L"bbb";
        ASSERT_EQ(fonts.begin()->Font().family_name, L"bbb");
    }

    //Const RangedTextStyle.
    {
        const RangedTextStyle& const_ranged_style{ ranged_style };
        auto& fonts = const_ranged_style.Fonts();
        ASSERT_EQ(fonts.FindItemContainsIndex(0), fonts.end());

        auto iterator = fonts.FindItemContainsIndex(3);
        ASSERT_NE(iterator, fonts.end());
        ASSERT_EQ(iterator->Range(), Range(2, 2));
        ASSERT_EQ(iterator->Font().family_name, L"bbb");
    }
}


TEST(RangedTextStyleTest, VisitItemsInRange) {

    RangedTextStyle ranged_style;
    ranged_style.SetFontInRange(Font{ L"1" }, Range{ 0, 2 });
    ranged_style.SetFontInRange(Font{ L"2" }, Range{ 5, 2 });
    ranged_style.SetFontInRange(Font{ L"3" }, Range{ 9, 2 });
    ranged_style.SetFontInRange(Font{ L"4" }, Range{ 12, 2 });

    std::vector<std::wstring> fonts;
    std::vector<Range> ranges;
    const auto visitor = [&fonts, &ranges](const RangedFontItem& item) {
        fonts.push_back(item.Font().family_name);
        ranges.push_back(item.Range());
    };

    const auto check_visit_result = [&fonts, &ranges](
        const std::vector<std::wstring>& expected_fonts,
        const std::vector<Range>& expected_ranges) {
        bool result = (fonts == expected_fonts) && (ranges == expected_ranges);
        fonts.clear();
        ranges.clear();
        return result;
    };

    {
        ranged_style.Fonts().VisitItemsInRange(Range{ 0, 20 }, visitor);
        std::vector<std::wstring> expected_fonts{
            L"1",
            L"2",
            L"3",
            L"4",
        };
        std::vector<Range> expected_ranges{
            Range{ 0, 2 },
            Range{ 5, 2 },
            Range{ 9, 2 },
            Range{ 12, 2 },
        };
        ASSERT_TRUE(check_visit_result(expected_fonts, expected_ranges));
    }

    {
        ranged_style.Fonts().VisitItemsInRange(Range::FromIndexPair(2, 12), visitor);
        std::vector<std::wstring> expected_fonts{
            L"2",
            L"3",
        };
        std::vector<Range> expected_ranges{
            Range{ 5, 2 },
            Range{ 9, 2 },
        };
        ASSERT_TRUE(check_visit_result(expected_fonts, expected_ranges));
    }

    {
        ranged_style.Fonts().VisitItemsInRange(Range::FromIndexPair(10, 13), visitor);
        std::vector<std::wstring> expected_fonts{
            L"3",
            L"4",
        };
        std::vector<Range> expected_ranges{
            Range{ 9, 2 },
            Range{ 12, 2 },
        };
        ASSERT_TRUE(check_visit_result(expected_fonts, expected_ranges));
    }
}


TEST(RangedTextStyleTest, VisitInlineObjectsInRange) {

    RangedTextStyle ranged_style;
    ranged_style.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 0, 2 });
    ranged_style.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 4, 2 });
    ranged_style.AttachInlineObjectToRange(Create<InlineObject>(), Range{ 8, 2 });

    std::vector<Range> ranges;
    const auto visitor = [&ranges](const auto& item) {
        ranges.push_back(item.Range());
    };

    const auto check_visit_result = [&ranges](const std::vector<Range>& expected_ranges) {
        bool result = ranges == expected_ranges;
        ranges.clear();
        return result;
    };

    {
        ranged_style.InlineObjects().VisitItemsInRange(Range{ 0, 10 }, visitor);
        std::vector<Range> expected_ranges{
            Range{ 0, 2 },
            Range{ 4, 2 },
            Range{ 8, 2 },
        };
        ASSERT_TRUE(check_visit_result(expected_ranges));
    }

    {
        ranged_style.InlineObjects().VisitItemsInRange(Range{ 4, 2 }, visitor);
        std::vector<Range> expected_ranges{
            Range{ 4, 2 },
        };
        ASSERT_TRUE(check_visit_result(expected_ranges));
    }

    //Only the items fully contained by the range will be visited.
    {
        ranged_style.InlineObjects().VisitItemsInRange(Range::FromIndexPair(1, 9), visitor);
        std::vector<Range> expected_ranges{
            Range{ 4, 2 },
        };
        ASSERT_TRUE(check_visit_result(expected_ranges));
    }
}