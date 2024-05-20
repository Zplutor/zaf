#include <gtest/gtest.h>
#include <zaf/control/textual/ranged_text_style.h>

using namespace zaf;
using namespace zaf::internal;
using namespace zaf::textual;

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