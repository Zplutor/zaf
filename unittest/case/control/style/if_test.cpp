#include <gtest/gtest.h>
#include <zaf/control/style/if.h>
#include <zaf/control/control.h>
#include <zaf/control/control_object.h>
#include <zaf/control/label.h>
#include <zaf/object/parsing/helpers.h>

using namespace zaf;

TEST(IfTest, Parse) {

    auto xaml = LR"(
        <If IsEnabled="true" IsVisible="true"> 
            <Set Background="#ffff00" />
        </If>
    )";

    auto if_statement = CreateObjectFromXaml<If>(xaml);
    ASSERT_NE(if_statement, nullptr);

    auto control = Create<Control>();
    control->SetIsEnabled(true);
    control->SetIsVisible(true);
    control->SetBackgroundColor(Color::FromRGB(0xffff00));

    const auto& conditions = if_statement->Conditions();
    ASSERT_EQ(conditions.size(), 2);
    auto iterator = conditions.begin();
    ASSERT_TRUE(iterator->IsSetIn(*control));
    ++iterator;
    ASSERT_TRUE(iterator->IsSetIn(*control));

    const auto& properties = if_statement->Result()->Properties();
    ASSERT_EQ(properties.size(), 1);
    ASSERT_TRUE(iterator->IsSetIn(*control));
}


TEST(IfTest, ApplyTo) {

    auto if_statement = Create<If>();
    if_statement->AddCondition(Control::Type::Instance()->IsEnabledProperty, Box(true));
    if_statement->AddCondition(Control::Type::Instance()->IsSelectedProperty, Box(true));
    if_statement->Result()->AddProperty(
        Control::Type::Instance()->BorderColorProperty,
        Box(Color::Blue()));

    //Sets to base class.
    {
        auto control = Create<Control>();
        control->SetIsEnabled(false);
        control->SetIsSelected(false);
        control->SetBorderColor(Color::Red());

        if_statement->ApplyTo(*control);
        ASSERT_EQ(control->BorderColor(), Color::Red());

        control->SetIsSelected(true);
        if_statement->ApplyTo(*control);
        ASSERT_EQ(control->BorderColor(), Color::Red());

        control->SetIsEnabled(true);
        if_statement->ApplyTo(*control);
        ASSERT_EQ(control->BorderColor(), Color::Blue());
    }

    //Sets to derived class.
    {
        auto label = Create<Label>();
        label->SetIsEnabled(false);
        label->SetIsSelected(false);
        label->SetBorderColor(Color::Red());

        if_statement->ApplyTo(*label);
        ASSERT_EQ(label->BorderColor(), Color::Red());

        label->SetIsSelected(true);
        if_statement->ApplyTo(*label);
        ASSERT_EQ(label->BorderColor(), Color::Red());

        label->SetIsEnabled(true);
        if_statement->ApplyTo(*label);
        ASSERT_EQ(label->BorderColor(), Color::Blue());
    }
}