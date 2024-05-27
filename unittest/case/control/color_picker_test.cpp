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


TEST(ColorPickerTest, Equality) {

    //Lambdas
    {
        ColorPicker picker1{ [](const Control&) { return Color::Red(); } };
        ColorPicker picker2{ [](const Control&) { return Color::Red(); } };
        ASSERT_FALSE(picker1 == picker2);
    }

    //Lambda and StaticColorPicker
    {
        ColorPicker picker1{ [](const Control&) { return Color::Red(); } };
        ColorPicker picker2{ StaticColorPicker{ Color::Red() } };
        ASSERT_FALSE(picker1 == picker2);
        ASSERT_FALSE(picker2 == picker1);
    }

    //StaticColorPickers
    {
        ColorPicker picker1{ StaticColorPicker{ Color::Red() } };
        ColorPicker picker2{ StaticColorPicker{ Color::Red() } };
        ASSERT_TRUE(picker1 == picker2);
    }
}