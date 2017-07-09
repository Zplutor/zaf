#include <gtest/gtest.h>
#include <zaf/control/check_box.h>
#include "utility/deserialize_utility.h"

TEST(CheckBox, Deserialize) {

    auto json = "{"
    "\"CanAutoChangeCheckState\": false,"
    "\"CanBeIndeterminate\": true,"
    "\"IsChecked\": true,"
    "\"Name\": \"yes\""
    "}";

    auto check_box = DeserializeObject<zaf::CheckBox>(json);
    ASSERT_EQ(check_box->CanAutoChangeCheckState(), false);
    ASSERT_EQ(check_box->CanBeIndeterminate(), true);
    ASSERT_EQ(check_box->IsChecked(), true);
    ASSERT_EQ(check_box->GetName(), L"yes");
}


TEST(CheckBox, DeserializeBoxBackgroundColor) {

    auto check_box = DeserializeObject<zaf::CheckBox>("{\"BoxBackgroundColor\": {\"R\": 0.98,\"G\": 0.89}}");
    ASSERT_EQ(check_box->GetBoxBackgroundColor(), zaf::Color(0.98f, 0.89f, 0));
}


TEST(CheckBox, DeserializeBoxBackgroundColorPicker) {

    auto json = "{\"BoxBackgroundColorPicker\": { \"Color\": {\"R\": 0.96,\"G\": 0.69}}}";
    auto check_box = DeserializeObject<zaf::CheckBox>(json);
    ASSERT_EQ(check_box->GetBoxBackgroundColor(), zaf::Color(0.96f, 0.69f, 0));
}


TEST(CheckBox, DeserializeBoxBorderColor) {

    auto check_box = DeserializeObject<zaf::CheckBox>("{\"BoxBorderColor\": { \"G\": 0.46, \"B\": 0.64 }}");
    ASSERT_EQ(check_box->GetBoxBorderColor(), zaf::Color(0, 0.46f, 0.64f));
}


TEST(CheckBox, DeserializeBoxBorderColorPicker) {

    auto json = "{\"BoxBorderColorPicker\": { \"Color\": {\"G\": 0.43, \"B\": 0.34 }}}";
    auto check_box = DeserializeObject<zaf::CheckBox>(json);
    ASSERT_EQ(check_box->GetBoxBorderColor(), zaf::Color(0, 0.43f, 0.34f));
}


TEST(CheckBox, DeserializeCheckState) {

    auto check_box = DeserializeObject<zaf::CheckBox>("{\"CheckState\": \"Checked\"}");
    ASSERT_EQ(check_box->GetCheckState(), zaf::CheckState::Checked);

    check_box = DeserializeObject<zaf::CheckBox>("{\"CheckState\": \"Indeterminate\"}");
    ASSERT_EQ(check_box->GetCheckState(), zaf::CheckState::Indeterminate);

    check_box = DeserializeObject<zaf::CheckBox>("{\"CheckState\": \"Unchecked\"}");
    ASSERT_EQ(check_box->GetCheckState(), zaf::CheckState::Unchecked);

    check_box = DeserializeObject<zaf::CheckBox>("{\"CheckState\": \"dafo23o\"}");
    ASSERT_EQ(check_box->GetCheckState(), zaf::CheckState::Unchecked);
}