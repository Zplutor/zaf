#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/control_object.h>
#include <zaf/control/label.h>
#include <zaf/control/style/set.h>
#include <zaf/object/parsing/helpers.h>

using namespace zaf;

TEST(SetTest, Parse) {

    auto xaml = LR"(<Set BackgroundColor="#00ff00" BorderColor="#ff00ff" />)";
    auto set = CreateObjectFromXaml<Set>(xaml);
    ASSERT_NE(set, nullptr);

    const auto& properties = set->Properties();
    ASSERT_EQ(properties.size(), 2);

    auto control = Create<Control>();
    control->SetBackgroundColor(Color::FromRGB(0x00ff00));
    control->SetBorderColor(Color::FromRGB(0xff00ff));

    auto iterator = properties.begin();
    ASSERT_TRUE(iterator->IsSetIn(*control));

    ++iterator;
    ASSERT_TRUE(iterator->IsSetIn(*control));
}


TEST(SetTest, ApplyTo) {

    auto set = Create<Set>();
    set->AddProperty(
        Control::Type::Instance()->BackgroundColorProperty, 
        Box(Color::Red()));

    set->AddProperty(Control::Type::Instance()->BorderColorProperty,
        Box(Color::Green()));

    //Sets to base class.
    {
        auto control = Create<Control>();
        set->ApplyTo(*control);
        ASSERT_EQ(control->BackgroundColor(), Color::Red());
        ASSERT_EQ(control->BorderColor(), Color::Green());
    }

    //Sets to derived class.
    {
        auto label = Create<Label>();
        set->ApplyTo(*label);
        ASSERT_EQ(label->BackgroundColor(), Color::Red());
        ASSERT_EQ(label->BorderColor(), Color::Green());
    }
}