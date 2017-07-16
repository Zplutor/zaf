#include <gtest/gtest.h>
#include <zaf/control/combo_box.h>
#include "utility/deserialize_utility.h"

TEST(ComboBox, Deserialize) {

    auto json = "{"
        "\"Name\":\"combobox\","
        "\"IsEditable\": true,"
        "\"DropDownButtonWidth\": 36,"
        "\"MinimumVisibleItemCount\": 10,"
        "\"MaximumVisibleItemCount\": 40"
        "}";
    auto combo_box = DeserializeObject<zaf::ComboBox>(json);
    ASSERT_EQ(combo_box->GetName(), L"combobox");
    ASSERT_EQ(combo_box->IsEditable(), true);
    ASSERT_EQ(combo_box->GetDropDownButtonWidth(), 36.f);
    ASSERT_EQ(combo_box->GetMinimumVisibleItemCount(), 10);
    ASSERT_EQ(combo_box->GetMaximumVisibleItemCount(), 40);
}


TEST(ComboBox, DeserializeDropDownButtonColor) {

    auto combo_box = DeserializeObject<zaf::ComboBox>("{\"DropDownButtonColor\": {\"G\": 0.81}}");
    ASSERT_EQ(combo_box->GetDropDownButtonColor(), zaf::Color(0, 0.81f, 0));
}


TEST(ComboBox, DeserializeDropDownButtonColorPicker) {

    auto combo_box = DeserializeObject<zaf::ComboBox>("{\"DropDownButtonColorPicker\": { \"Color\": { \"G\": 0.82}}}");
    ASSERT_EQ(combo_box->GetDropDownButtonColor(), zaf::Color(0, 0.82f, 0));
}


TEST(ComboBox, DeserializeDropDownListBox) {

    auto combo_box = DeserializeObject<zaf::ComboBox>("{\"DropDownListBox\": { \"Name\": \"dropdownlistbox\" }}");
    ASSERT_EQ(combo_box->GetDropDownListBox()->GetName(), L"dropdownlistbox");
}


TEST(ComboBox, DeserializeEditTextBox) {

    auto combo_box = DeserializeObject<zaf::ComboBox>("{\"EditTextBox\": {\"Name\": \"edittextbox\"}}");
    ASSERT_EQ(combo_box->GetEditTextBox()->GetName(), L"edittextbox");
}