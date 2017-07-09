#include <gtest/gtest.h>
#include <zaf/control/radio_button.h>
#include "utility/deserialize_utility.h"

TEST(RadioButton, DeserializeCanAutoSelect) {

    auto json = "{\"CanAutoSelect\": false, \"Name\": \"Radio\"}";
    auto control = DeserializeObject<zaf::RadioButton>(json);
    ASSERT_EQ(control->CanAutoSelect(), false);
    ASSERT_EQ(control->GetName(), L"Radio");
}


TEST(RadioButton, DeserializeRadioBackgroundColor) {

    auto control = DeserializeObject<zaf::RadioButton>("{\"RadioBackgroundColor\":{\"G\": 0.23}}");
    ASSERT_EQ(control->GetRadioBackgroundColor(), zaf::Color(0, 0.23f, 0));
}


TEST(RadioButton, DeserializeRadioBackgroundColorPicker) {

    auto control = DeserializeObject<zaf::RadioButton>("{\"RadioBackgroundColorPicker\":{\"Color\": { \"G\":0.32 }}}");
    ASSERT_EQ(control->GetRadioBackgroundColor(), zaf::Color(0, 0.32f, 0));
}


TEST(RadioButton, DeserializeRadioBorderColor) {

    auto control = DeserializeObject<zaf::RadioButton>("{\"RadioBorderColor\":{ \"G\":0.23, \"B\": 0.98 }}");
    ASSERT_EQ(control->GetRadioBorderColor(), zaf::Color(0, 0.23f, 0.98f));
}


TEST(RadioButton, DeserializeRadioBorderColorPicker) {

    auto control = DeserializeObject<zaf::RadioButton>("{\"RadioBorderColorPicker\":{\"Color\": { \"G\":0.32, \"B\": 0.89 }}}");
    ASSERT_EQ(control->GetRadioBorderColor(), zaf::Color(0, 0.32f, 0.89f));
}

