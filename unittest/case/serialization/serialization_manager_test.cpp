#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/control/button.h>
#include <zaf/control/check_box.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/combo_box.h>
#include <zaf/control/control.h>
#include <zaf/control/image_box.h>
#include <zaf/control/label.h>
#include <zaf/control/list_box.h>
#include <zaf/control/radio_button.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/split_control.h>
#include <zaf/control/text_box.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/rounded_rect.h>
#include <zaf/serialization/serializable_type.h>
#include <zaf/serialization/serialization_manager.h>

TEST(SerializationManager, BuiltInTypes) {

    struct {
        const wchar_t* name;
        std::shared_ptr<zaf::SerializableObject> object;
    } built_in_type_infos[] = {
    
        L"Arrow.ScrollBar", std::make_shared<zaf::ScrollBar::Arrow>(),
        L"Button", std::make_shared<zaf::Button>(),
        L"CheckBox", std::make_shared<zaf::CheckBox>(),
        L"ClickableControl", std::make_shared<zaf::ClickableControl>(),
        L"Color", std::make_shared<zaf::Color>(),
        L"ConstantColorPicker", std::make_shared<zaf::ConstantColorPicker>(),
        L"ComboBox", std::make_shared<zaf::ComboBox>(),
        L"Control", std::make_shared<zaf::Control>(),
        L"DropDownListBox.ComboBox", std::make_shared<zaf::ComboBox::DropDownListBox>(),
        L"Ellipse", std::make_shared<zaf::Ellipse>(),
        L"ImageBox", std::make_shared<zaf::ImageBox>(),
        L"Label", std::make_shared<zaf::Label>(),
        L"ListBox", std::make_shared<zaf::ListBox>(),
        L"ListControl", std::make_shared<zaf::ListControl>(),
        L"Point", std::make_shared<zaf::Point>(),
        L"RadioButton", std::make_shared<zaf::RadioButton>(),
        L"Rect", std::make_shared<zaf::Rect>(),
        L"RoundedRect", std::make_shared<zaf::RoundedRect>(),
        L"ScrollableControl", std::make_shared<zaf::ScrollableControl>(),
        L"ScrollBar", std::make_shared<zaf::ScrollBar>(),
        L"SplitControl", std::make_shared<zaf::SplitControl>(),
        L"Size", std::make_shared<zaf::Size>(),
        L"TextBox", std::make_shared<zaf::TextBox>(),
        L"TextualControl", std::make_shared<zaf::TextualControl>(),
        L"Thumb.ScrollBar", std::make_shared<zaf::ScrollBar::Thumb>(),
    };

    for (const auto& each_info : built_in_type_infos) {

        auto type = zaf::GetSerializationManager()->GetType(each_info.name);
        ASSERT_NE(type, nullptr);
        ASSERT_STREQ(type->GetName(), each_info.name);
        ASSERT_EQ(type->GetTypeInfo(), typeid(*each_info.object));

        auto instance = type->CreateInstance();
        ASSERT_NE(instance, nullptr);
        ASSERT_EQ(typeid(*instance), typeid(*each_info.object));
    }
}