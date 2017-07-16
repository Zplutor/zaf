#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/json/json_reader.h>
#include <zaf/serialization/properties.h>

namespace {

bool DeserializeControl(const std::string& json, const std::function<bool(const zaf::Control& control)>& checker) {

    zaf::JsonReader json_reader;
    auto data_node = json_reader.Read(json);
    if (data_node == nullptr) {
        return false;
    }

    auto control = zaf::Create<zaf::Control>();
    bool is_succeeded = control->Deserialize(*data_node);
    if (!is_succeeded) {
        return false;
    }

    return checker(*control);
}

}


TEST(Control, DeserializeRect) {

    auto json = "{"
        "\"Rect\": {"
        "\"Position\":{\"X\":2, \"Y\":3}, "
        "\"Size\":{ \"Width\":4,\"Height\":5, }"
        "}}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetRect() == zaf::Rect(2, 3, 4, 5);
    }));
}


TEST(Control, DeserializeBorder) {

    auto json = "{\"Border\": {\"All\":5}}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetBorder() == zaf::Frame(5);
    }));
}


TEST(Control, DeserializePadding) {

    auto json = "{\"Padding\": {\"All\":6.6}}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetPadding() == zaf::Frame(6.6f);
    }));
}


TEST(Control, DeserializeBackgroundColor) {

    auto color_node = zaf::DataNode::CreateObject();
    color_node->AddChild(zaf::property::R, zaf::DataNode::CreateNumber(0.1));
    color_node->AddChild(zaf::property::G, zaf::DataNode::CreateNumber(0.2));
    color_node->AddChild(zaf::property::B, zaf::DataNode::CreateNumber(0.3));

    auto data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::BackgroundColor, color_node);

    auto control = zaf::Create<zaf::Control>();
    bool is_succeeded = control->Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    auto color = control->GetBackgroundColor();
    ASSERT_EQ(color.r, 0.1f);
    ASSERT_EQ(color.g, 0.2f);
    ASSERT_EQ(color.b, 0.3f);
}


TEST(Control, DeserializeBackgroundColorPicker) {

    auto json = 
        "{"
        "\"BackgroundColorPicker\": {"
        "\"Color\":{ \"B\": 0.44 }"
        "}"
        "}";

    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetBackgroundColor() == zaf::Color(0, 0, 0.44f);
    }));
}


TEST(Control, DeserializeBorderColor) {

    auto json = 
        "{"
        "\"BorderColor\": {"
        "\"R\": 0.4,"
        "\"G\": 0.5,"
        "\"B\": 0.6,"
        "}"
        "}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        zaf::Color expected(0.4f, 0.5f, 0.6f);
        return control.GetBorderColor() == expected;
    }));
}


TEST(Control, DeserializeBorderColorPicker) {

    auto json =
        "{"
        "\"BorderColorPicker\": {"
        "\"Color\":{ \"B\": 0.45 }"
        "}"
        "}";

    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetBorderColor() == zaf::Color(0, 0, 0.45f);
    }));
}


TEST(Control, DeserializeIsVisible) {

    auto json = "{\"IsVisible\": true}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.IsVisible();
    }));
}


TEST(Control, DeserializeIsEnabled) {

    auto json = "{\"IsEnabled\": false}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.IsEnabled() == false;
    }));
}


TEST(Control, DeserializeCanFocused) {

    auto json = "{\"CanFocused\": true}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.CanFocused();
    }));
}


TEST(Control, DeserializeName) {

    auto json = "{\"Name\": \"NNNN\"}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetName() == L"NNNN";
    }));
}


TEST(Control, DeserializeMinimumWidth) {

    auto json = "{\"MinimumWidth\": 3}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetMinimumWidth() == 3;
    }));
}


TEST(Control, DeserializeMaximumWidth) {

    auto json = "{\"MaximumWidth\": 4}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetMaximumWidth() == 4;
    }));
}


TEST(Control, DeserializeMinimumHeight) {

    auto json = "{\"MinimumHeight\": 6}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetMinimumHeight() == 6;
    }));
}


TEST(Control, DeserializeMaximumHeight) {

    auto json = "{\"MaximumHeight\": 7}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {
        return control.GetMaximumHeight() == 7;
    }));
}


TEST(Control, DeserializeChildren) {
   
    auto json =     
        "{\"Children\": ["
        "{\"Type\": \"Control\"},"
        "{\"Type\": \"Control\", \"Name\":\"haha\"} "
        "]}";
    ASSERT_TRUE(DeserializeControl(json, [](const zaf::Control& control) {

        const auto& children = control.GetChildren();
        if (children.size() != 2) {
            return false;
        }
        
        if (! children[0]->GetName().empty()) {
            return false;
        }

        return children[1]->GetName() == L"haha";
    }));

    //Previous children should be clear when deserializing an empty array.
    auto data_node = zaf::DataNode::CreateObject();
    data_node->AddChild(zaf::property::Children, zaf::DataNode::CreateArray());

    auto control = zaf::Create<zaf::Control>();
    control->AddChild(zaf::Create<zaf::Control>());
    bool is_succeeded = control->Deserialize(*data_node);
    ASSERT_TRUE(is_succeeded);
    ASSERT_TRUE(control->GetChildren().empty());
}