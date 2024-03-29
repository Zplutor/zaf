#include <gtest/gtest.h>
#include <zaf/control/textual/ranged_text_style.h>

using namespace zaf;
using namespace zaf::internal;
using namespace zaf::textual;

static_assert(!std::is_copy_constructible_v<RangedFontItem<false>>);
static_assert(!std::is_copy_assignable_v<RangedFontItem<false>>);
static_assert(std::is_move_constructible_v<RangedFontItem<false>>);
static_assert(!std::is_move_assignable_v<RangedFontItem<false>>);

static_assert(!std::is_copy_constructible_v<RangedFontItem<true>>);
static_assert(!std::is_copy_assignable_v<RangedFontItem<true>>);
static_assert(std::is_move_constructible_v<RangedFontItem<true>>);
static_assert(!std::is_move_assignable_v<RangedFontItem<true>>);


TEST(RangedTextStyleTest, RangedTypesCheck) {

    static_assert(std::is_same_v<
        RangedTextStyle::FontEnumerator::value_type,
        RangedFontItem<false>>);

    static_assert(std::is_same_v<
        RangedTextStyle::FontEnumerator::iterator::value_type,
        RangedFontItem<false>>);

    static_assert(std::is_same_v<
        RangedTextStyle::FontEnumerator::const_iterator::value_type,
        RangedFontItem<true>>);

    static_assert(std::is_same_v<
        RangedTextStyle::ConstFontEnumerator::value_type,
        RangedFontItem<true>>);

    static_assert(std::is_same_v<
        RangedTextStyle::ConstFontEnumerator::iterator::value_type,
        RangedFontItem<true>>);

    static_assert(std::is_same_v<
        RangedTextStyle::ConstFontEnumerator::const_iterator::value_type,
        RangedFontItem<true>>);

    {
        RangedTextStyle ranged_style;
        auto iterator = ranged_style.Fonts().begin();
        static_assert(std::is_same_v<decltype(iterator)::value_type, RangedFontItem<false>>);
    }

    {
        RangedTextStyle ranged_style;
        auto iterator = ranged_style.Fonts().cbegin();
        static_assert(std::is_same_v<decltype(iterator)::value_type, RangedFontItem<true>>);
    }

    {
        const RangedTextStyle ranged_style;
        auto iterator = ranged_style.Fonts().begin();
        static_assert(std::is_same_v<decltype(iterator)::value_type, RangedFontItem<true>>);
    }
}


TEST(RangedTextStyleTest, EnumerateFonts) {

    RangedTextStyle ranged_style;
    ranged_style.SetFontInRange(Font{ L"aa" }, Range{ 0, 1 });
    ranged_style.SetFontInRange(Font{ L"bb" }, Range{ 5, 2 });
    ranged_style.SetFontInRange(Font{ L"cc" }, Range{ 10, 3 });

    //for (auto& each_item : ranged_style.Fonts()) {
    //}
}