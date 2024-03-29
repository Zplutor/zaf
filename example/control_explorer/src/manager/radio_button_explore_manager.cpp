#include "manager/radio_button_explore_manager.h"
#include <zaf/control/layout/linear_layouter.h>

std::shared_ptr<zaf::Control> RadioButtonExploreManager::CreateExploredControl() {

    auto container = zaf::Create<zaf::Control>();
    container->SetSize(zaf::Size(200, 90));
    container->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

    auto group = std::make_shared<zaf::RadioButton::Group>();

    target_radio_button_ = zaf::Create<zaf::RadioButton>();
    target_radio_button_->SetText(L"Radio button");
    target_radio_button_->SetAssociatedGroup(group);
    container->AddChild(target_radio_button_);

    auto other_radio_button1 = zaf::Create<zaf::RadioButton>();
    other_radio_button1->SetText(L"Other radio button 1");
    other_radio_button1->SetAssociatedGroup(group);
    container->AddChild(other_radio_button1);

    auto other_radio_button2 = zaf::Create<zaf::RadioButton>();
    other_radio_button2->SetText(L"Other radio button 2");
    other_radio_button2->SetAssociatedGroup(group);
    container->AddChild(other_radio_button2);

    return container;
}
