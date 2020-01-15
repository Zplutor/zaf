#include <gtest/gtest.h>
#include <zaf/control/check_box.h>
#include <zaf/parsing/parsers/check_box_parser.h>
#include "utility.h"

using namespace zaf;

TEST(CheckBoxParser, ParseCanAutoChangeCheckState) {
    ASSERT_TRUE(TestBooleanProperty<CheckBox>("CanAutoChangeCheckState", [](CheckBox& check_box) {
        return check_box.CanAutoChangeCheckState();
    }));
}


TEST(CheckBoxParser, ParseCanBeIndeterminate) {
    ASSERT_TRUE(TestBooleanProperty<CheckBox>("CanBeIndeterminate", [](CheckBox& check_box) {
        return check_box.CanBeIndeterminate();
    }));
}


TEST(CheckBoxParser, ParseIsChecked) {
    ASSERT_TRUE(TestBooleanProperty<CheckBox>("IsChecked", [](CheckBox& check_box) {
        return check_box.IsChecked();
    }));
}


TEST(CheckBoxParser, ParseCheckState) {

    auto result = TestEnumProperty<CheckBox, CheckState>(
        "CheckState", 
        {
            { CheckState::Checked, "Checked" },
            { CheckState::Unchecked, "Unchecked" },
            { CheckState::Indeterminate, "Indeterminate" },
        },
        [](CheckBox& check_box) {
            return check_box.GetCheckState();
        }
    );
    ASSERT_TRUE(result);
}