#include "manager/radio_button_explore_manager.h"
#include <zaf/control/layout/linear_layouter.h>
#include "property/check_box_property_item.h"

std::shared_ptr<zaf::Control> RadioButtonExploreManager::CreateExploredControl() {

    auto container = zaf::Create<zaf::Control>();
    container->SetSize(zaf::Size(200, 90));
    container->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

    auto group = std::make_shared<zaf::RadioButton::Group>();

    target_radio_button_ = zaf::Create<zaf::RadioButton>();
    target_radio_button_->SetText(L"Radio button");
    target_radio_button_->SetGroup(group);
    container->AddChild(target_radio_button_);

    auto other_radio_button1 = zaf::Create<zaf::RadioButton>();
    other_radio_button1->SetText(L"Other radio button 1");
    other_radio_button1->SetGroup(group);
    container->AddChild(other_radio_button1);

    auto other_radio_button2 = zaf::Create<zaf::RadioButton>();
    other_radio_button2->SetText(L"Other radio button 2");
    other_radio_button2->SetGroup(group);
    container->AddChild(other_radio_button2);

    return container;
}


void RadioButtonExploreManager::CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    __super::CreatePropertyItems(items);

    items.insert(items.end(), {
        CreateCanAutoSelectPropertyItem(),
        CreateIsSelectedPropertyItem(),
    });
}


std::shared_ptr<PropertyItem> RadioButtonExploreManager::CreateCanAutoSelectPropertyItem() {

    auto radio_button = GetRadioButton();
    return CreateCheckBoxPropertyItem(
        L"Can auto select",
        [radio_button]() { return radio_button->CanAutoSelect(); },
        [radio_button](bool value) { radio_button->SetCanAutoSelect(value); },
        nullptr);
}


std::shared_ptr<PropertyItem> RadioButtonExploreManager::CreateIsSelectedPropertyItem() {

    auto radio_button = GetRadioButton();
    return CreateCheckBoxPropertyItem(
        L"Is selected",
        [radio_button]() { return radio_button->IsSelected(); },
        [radio_button](bool value) { if (value) { radio_button->SetSelected(); } },
        [radio_button](const std::function<void()>& callback) {

            radio_button->Subscriptions() +=
                radio_button->SelectStateChangeEvent().Subscribe(std::bind(callback));
        });
}