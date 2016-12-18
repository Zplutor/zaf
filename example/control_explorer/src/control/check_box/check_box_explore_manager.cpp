#include "control/check_box/check_box_explore_manager.h"
#include <zaf/control/check_box.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/radio_button.h>
#include <zaf/creation.h>
#include "control/common.h"
#include "control/control_operate_panels_creator.h"
#include "control/textual_control_operate_panels_creator.h"

static std::shared_ptr<zaf::Control> CreateCanAutoChangeCheckStateOperatePanel(const std::shared_ptr<zaf::CheckBox>& check_box);
static std::shared_ptr<zaf::Control> CreateCanBeIndeterminateOperatePanel(const std::shared_ptr<zaf::CheckBox>& check_box);
static std::shared_ptr<zaf::Control> CreateIsCheckedOperatePanel(const std::shared_ptr<zaf::CheckBox>& check_box);
static std::shared_ptr<zaf::Control> CreateCheckStateOperatePanel(const std::shared_ptr<zaf::CheckBox>& check_box);

void CheckBoxExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetSize(zaf::Size(200, 30));
    check_box->SetText(L"Check box");

    control = check_box;

    CreateControlOperatePanels(check_box, operate_panels);
    CreateTextualControlOperatePanels(check_box, operate_panels);
    operate_panels.push_back(CreateCanAutoChangeCheckStateOperatePanel(check_box));
    operate_panels.push_back(CreateIsCheckedOperatePanel(check_box));
    operate_panels.push_back(CreateCanBeIndeterminateOperatePanel(check_box));
    operate_panels.push_back(CreateCheckStateOperatePanel(check_box));
}


static std::shared_ptr<zaf::Control> CreateCanAutoChangeCheckStateOperatePanel(const std::shared_ptr<zaf::CheckBox>& check_box) {

    auto can_auto_change_check_state = zaf::Create<zaf::CheckBox>();
    can_auto_change_check_state->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    can_auto_change_check_state->SetText(L"Can auto change check state");
    can_auto_change_check_state->GetCheckStateChangeEvent().AddListener([check_box](const std::shared_ptr<zaf::CheckBox>& can_auto_change_check_state) {
        check_box->SetCanAutoChangeCheckState(can_auto_change_check_state->IsChecked());
    });
    can_auto_change_check_state->SetIsChecked(check_box->CanAutoChangeCheckState());

    return can_auto_change_check_state;
}


static std::shared_ptr<zaf::Control> CreateCanBeIndeterminateOperatePanel(const std::shared_ptr<zaf::CheckBox>& check_box) {

    auto can_be_indeterminate_check_box = zaf::Create<zaf::CheckBox>();
    can_be_indeterminate_check_box->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    can_be_indeterminate_check_box->SetText(L"Can be indeterminate");
    can_be_indeterminate_check_box->GetCheckStateChangeEvent().AddListener([check_box](const std::shared_ptr<zaf::CheckBox>& can_be_indeterminate) {
        check_box->SetCanBeIndeterminate(can_be_indeterminate->IsChecked());
    });
    can_be_indeterminate_check_box->SetIsChecked(check_box->CanBeIndeterminate());

    return can_be_indeterminate_check_box;
}


static std::shared_ptr<zaf::Control> CreateIsCheckedOperatePanel(const std::shared_ptr<zaf::CheckBox>& check_box) {

    auto is_checked_check_box = zaf::Create<zaf::CheckBox>();
    is_checked_check_box->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    is_checked_check_box->SetText(L"Is checked");
    is_checked_check_box->GetCheckStateChangeEvent().AddListener([check_box](const std::shared_ptr<zaf::CheckBox>& is_checked) {
        check_box->SetIsChecked(is_checked->IsChecked());
    });
    is_checked_check_box->SetIsChecked(check_box->IsChecked());

    check_box->GetCheckStateChangeEvent().AddListener([is_checked_check_box](const std::shared_ptr<zaf::CheckBox>& check_box) {
        is_checked_check_box->SetIsChecked(check_box->IsChecked());
    });

    return is_checked_check_box;
}


static std::shared_ptr<zaf::Control> CreateCheckStateOperatePanel(const std::shared_ptr<zaf::CheckBox>& check_box) {

    auto panel = zaf::Create<zaf::Control>();
    panel->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    panel->SetLayouter(zaf::GetHorizontalArrayLayouter());

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Check state");
    panel->AddChild(label);

    auto group = std::make_shared<zaf::RadioButton::Group>();

    auto create_radio_button = [&](const std::wstring& text, zaf::CheckState check_state) {
    
        auto radio_button = zaf::Create<zaf::RadioButton>();
        radio_button->SetText(text);
        radio_button->SetGroup(group);
        radio_button->GetSelectStateChangeEvent().AddListener([check_box, check_state](const std::shared_ptr<zaf::RadioButton>& radio_button) {
            if (radio_button->IsSelected()) {
                check_box->SetCheckState(check_state);
            }
        });
        
        auto update_radio_button = [radio_button, check_state](const std::shared_ptr<zaf::CheckBox>& check_box) {
            if (check_box->GetCheckState() == check_state) {
                radio_button->SetSelected();
            }
        };
        
        update_radio_button(check_box);

        check_box->GetCheckStateChangeEvent().AddListener([update_radio_button](const std::shared_ptr<zaf::CheckBox>& check_box) {
            update_radio_button(check_box);
        });

        panel->AddChild(radio_button);
    };

    create_radio_button(L"Checked", zaf::CheckState::Checked);
    create_radio_button(L"Indeterminate", zaf::CheckState::Indeterminate);
    create_radio_button(L"Unchecked", zaf::CheckState::Unchecked);

    return panel;
}