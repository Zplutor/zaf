#include "control/radio_button_explore_manager.h"
#include <zaf/control/layout/array_layouter.h>
#include <zaf/creation.h>
#include "operate/radio_button_operate_item_factory.h"

static std::shared_ptr<zaf::RadioButton> CreateRadioButton(std::shared_ptr<zaf::Control>& container);

void RadioButtonExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_items) {

    auto radio_button = CreateRadioButton(control);

    RadioButtonOperateItemFactory factory;
    factory.SetTargetControl(radio_button);
    operate_items = factory.CreateOperateItems();
}


static std::shared_ptr<zaf::RadioButton> CreateRadioButton(std::shared_ptr<zaf::Control>& container) {

    container = zaf::Create<zaf::Control>();
    container->SetSize(zaf::Size(200, 90));
    container->SetLayouter(zaf::GetVerticalArrayLayouter());

    auto group = std::make_shared<zaf::RadioButton::Group>();

    auto radio_button = zaf::Create<zaf::RadioButton>();
    radio_button->SetText(L"Radio button");
    radio_button->SetGroup(group);
    container->AddChild(radio_button);

    auto other_radio_button1 = zaf::Create<zaf::RadioButton>();
    other_radio_button1->SetText(L"Other radio button 1");
    other_radio_button1->SetGroup(group);
    container->AddChild(other_radio_button1);

    auto other_radio_button2 = zaf::Create<zaf::RadioButton>();
    other_radio_button2->SetText(L"Other radio button 2");
    other_radio_button2->SetGroup(group);
    container->AddChild(other_radio_button2);

    return radio_button;
}