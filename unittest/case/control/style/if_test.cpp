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
            <Color>#ff00ff</Color>
        </If>
    )";

    auto if_statement = CreateObjectFromXaml<If>(xaml);
    ASSERT_NE(if_statement, nullptr);

    auto control = Create<Control>();
    control->SetIsEnabled(true);
    control->SetIsVisible(true);

    const auto& conditions = if_statement->Conditions();
    ASSERT_EQ(conditions.size(), 2);
    auto iterator = conditions.begin();
    ASSERT_TRUE(iterator->IsSetIn(*control));
    ++iterator;
    ASSERT_TRUE(iterator->IsSetIn(*control));

    auto result_name = if_statement->Result()->DynamicType()->Name();
    ASSERT_EQ(result_name, L"Color");

    auto result = if_statement->Evaluate(*control);
    ASSERT_NE(result, nullptr);
    auto color = As<Color>(result);
    ASSERT_EQ(*color, Color::FromRGB(0xff00ff));
}


