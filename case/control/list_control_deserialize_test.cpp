#include <gtest/gtest.h>
#include <zaf/control/list_control.h>
#include <zaf/serialization/properties.h>
#include "utility/deserialize_utility.h"

TEST(ListControl, Deserialize) {

    auto json = "{\"Name\": \"ListControl\"}";
    auto control = DeserializeObject<zaf::ListControl>(json);
    ASSERT_EQ(control->GetName(), L"ListControl");
}


TEST(ListControl, DeserializeSelectionMode) {

    auto test = [](const std::string& text, zaf::ListControl::SelectionMode value) {
    
        std::string json("{\"SelectionMode\":\"");
        json.append(text).append("\"}");

        auto control = DeserializeObject<zaf::ListControl>(json);
        return control->GetSelectionMode() == value;
    };

    ASSERT_TRUE(test("None", zaf::ListControl::SelectionMode::None));
    ASSERT_TRUE(test("Single", zaf::ListControl::SelectionMode::Single));
    ASSERT_TRUE(test("SimpleMultiple", zaf::ListControl::SelectionMode::SimpleMultiple));
    ASSERT_TRUE(test("ExtendedMultiple", zaf::ListControl::SelectionMode::ExtendedMultiple));
    ASSERT_TRUE(test("safoi32oi23khkl", zaf::ListControl::SelectionMode::Single));
}