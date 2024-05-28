#include <gtest/gtest.h>
#include <zaf/control/color_picker.h>

using namespace zaf;

static_assert(std::is_copy_constructible_v<ColorPicker>);
static_assert(std::is_copy_assignable_v<ColorPicker>);
static_assert(std::is_move_constructible_v<ColorPicker>);
static_assert(std::is_move_assignable_v<ColorPicker>);

TEST(ColorPickerTest, ConvertFromLambda) {

    {
        ColorPicker color_picker{ [](const Control&) { return Color::Black(); } };
        ASSERT_TRUE(color_picker);
    }

    {
        ColorPicker color_picker = [](const Control&) { return Color::Black(); };
        ASSERT_TRUE(color_picker);
    }

    {
        ColorPicker color_picker;
        color_picker = [](const Control&) { return Color::Black(); };
        ASSERT_TRUE(color_picker);
    }

    {
        ColorPicker color_picker;
        auto lambda = [](const Control&) { return Color::Black(); };
        color_picker = std::move(lambda);
        ASSERT_TRUE(color_picker);
    }
}

