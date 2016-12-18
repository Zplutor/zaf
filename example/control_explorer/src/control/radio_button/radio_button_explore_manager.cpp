#include "control/radio_button/radio_button_explore_manager.h"
#include <zaf/control/check_box.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/radio_button.h>
#include <zaf/creation.h>
#include "control/common.h"
#include "control/control_operate_panels_creator.h"
#include "control/textual_control_operate_panels_creator.h"

static std::shared_ptr<zaf::RadioButton> CreateRadioButton(std::shared_ptr<zaf::Control>& container);
static std::shared_ptr<zaf::Control> CreateCanAutoSelectOperatePanel(const std::shared_ptr<zaf::RadioButton>& radio_button);
static std::shared_ptr<zaf::Control> CreateSelectedOperatePanel(const std::shared_ptr<zaf::RadioButton>& radio_button);

void RadioButtonExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    auto radio_button = CreateRadioButton(control);

    CreateControlOperatePanels(radio_button, operate_panels);
    CreateTextualControlOperatePanels(radio_button, operate_panels);
    operate_panels.push_back(CreateCanAutoSelectOperatePanel(radio_button));
    operate_panels.push_back(CreateSelectedOperatePanel(radio_button));
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


static std::shared_ptr<zaf::Control> CreateCanAutoSelectOperatePanel(const std::shared_ptr<zaf::RadioButton>& radio_button) {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    check_box->SetText(L"Can auto select");
    check_box->GetCheckStateChangeEvent().AddListener([radio_button](const std::shared_ptr<zaf::CheckBox>& check_box) {
        radio_button->SetCanAutoSelect(check_box->IsChecked());
    });
    check_box->SetIsChecked(radio_button->CanAutoSelect());

    return check_box;
}


static std::shared_ptr<zaf::Control> CreateSelectedOperatePanel(const std::shared_ptr<zaf::RadioButton>& radio_button) {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    check_box->SetText(L"Is selected");
    check_box->SetCanAutoChangeCheckState(false);
    check_box->GetClickEvent().AddListener([radio_button](const std::shared_ptr<zaf::ClickableControl>& clickable_control) {

        radio_button->SetSelected();

        auto check_box = std::dynamic_pointer_cast<zaf::CheckBox>(clickable_control);
        check_box->SetIsChecked(radio_button->IsSelected());
    });
    check_box->SetIsChecked(radio_button->IsSelected());

    radio_button->GetSelectStateChangeEvent().AddListener([check_box](const std::shared_ptr<zaf::RadioButton>& radio_button) {
        check_box->SetIsChecked(radio_button->IsSelected());
    });

    return check_box;
}