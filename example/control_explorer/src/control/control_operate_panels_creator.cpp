#include "control_operate_panels_creator.h"
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/control/button.h>
#include <zaf/control/check_box.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include "control/common.h"

static std::shared_ptr<zaf::Control> CreateWidthPanel(const std::shared_ptr<zaf::Control>& control);
static std::shared_ptr<zaf::Control> CreateHeightPanel(const std::shared_ptr<zaf::Control>& control);
static std::shared_ptr<zaf::Control> CreateEnablePanel(const std::shared_ptr<zaf::Control>& control);
static std::shared_ptr<zaf::Control> CreateVisiblePanel(const std::shared_ptr<zaf::Control>& control);

void CreateControlOperatePanels(
    const std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    operate_panels.push_back(CreateWidthPanel(control));
    operate_panels.push_back(CreateHeightPanel(control));
    operate_panels.push_back(CreateEnablePanel(control));
    operate_panels.push_back(CreateVisiblePanel(control));
}


static std::shared_ptr<zaf::Control> CreateWidthPanel(const std::shared_ptr<zaf::Control>& control) {

    auto panel = CreateOperateContainerPanel(1);

    auto width_label = zaf::Create<zaf::Label>();
    width_label->SetText(L"Width");
    panel->AddChild(width_label);

    auto width_text_box = zaf::Create<zaf::TextBox>();
    width_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    width_text_box->SetText(zaf::ToWideString(control->GetWidth(), zaf::ToStringOptions().Precision(2)));
    panel->AddChild(width_text_box);

    auto min_width_label = zaf::Create<zaf::Label>();
    min_width_label->SetText(L"Minimum width");
    panel->AddChild(min_width_label);

    auto min_width_text_box = zaf::Create<zaf::TextBox>();
    min_width_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    min_width_text_box->SetText(zaf::ToWideString(control->GetMinimumWidth(), zaf::ToStringOptions().Precision(2)));
    panel->AddChild(min_width_text_box);

    auto max_width_label = zaf::Create<zaf::Label>();
    max_width_label->SetText(L"Maximum width");
    panel->AddChild(max_width_label);

    auto max_width_text_box = zaf::Create<zaf::TextBox>();
    max_width_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    max_width_text_box->SetText(zaf::ToWideString(control->GetMaximumWidth(), zaf::ToStringOptions().Precision(2)));
    panel->AddChild(max_width_text_box);

    auto set_button = CreateSetButton();
    set_button->GetClickEvent().AddListener(std::bind([width_text_box, min_width_text_box, max_width_text_box, control]() {
    
        auto width = zaf::ToNumeric<float>(width_text_box->GetText());
        auto min_width = zaf::ToNumeric<float>(min_width_text_box->GetText());
        auto max_width = zaf::ToNumeric<float>(max_width_text_box->GetText());
        control->SetWidth(width);
        control->SetMinimumWidth(min_width);
        control->SetMaximumWidth(max_width);
    }));
    panel->AddChild(set_button);

    return panel;
}


static std::shared_ptr<zaf::Control> CreateHeightPanel(const std::shared_ptr<zaf::Control>& control) {

    auto panel = CreateOperateContainerPanel(1);

    auto height_label = zaf::Create<zaf::Label>();
    height_label->SetText(L"Height");
    panel->AddChild(height_label);

    auto height_text_box = zaf::Create<zaf::TextBox>();
    height_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    height_text_box->SetText(zaf::ToWideString(control->GetHeight(), zaf::ToStringOptions().Precision(2)));
    panel->AddChild(height_text_box);

    auto min_height_label = zaf::Create<zaf::Label>();
    min_height_label->SetText(L"Minimum height");
    panel->AddChild(min_height_label);

    auto min_height_text_box = zaf::Create<zaf::TextBox>();
    min_height_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    min_height_text_box->SetText(zaf::ToWideString(control->GetMinimumHeight(), zaf::ToStringOptions().Precision(2)));
    panel->AddChild(min_height_text_box);

    auto max_height_label = zaf::Create<zaf::Label>();
    max_height_label->SetText(L"Maximum height");
    panel->AddChild(max_height_label);

    auto max_height_text_box = zaf::Create<zaf::TextBox>();
    max_height_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    max_height_text_box->SetText(zaf::ToWideString(control->GetMaximumHeight(), zaf::ToStringOptions().Precision(2)));
    panel->AddChild(max_height_text_box);

    auto set_button = CreateSetButton();
    set_button->GetClickEvent().AddListener(std::bind([height_text_box, min_height_text_box, max_height_text_box, control]() {

        auto height = zaf::ToNumeric<float>(height_text_box->GetText());
        auto min_height = zaf::ToNumeric<float>(min_height_text_box->GetText());
        auto max_height = zaf::ToNumeric<float>(max_height_text_box->GetText());
        control->SetHeight(height);
        control->SetMinimumHeight(min_height);
        control->SetMaximumHeight(max_height);
    }));
    panel->AddChild(set_button);

    return panel;
}


static std::shared_ptr<zaf::Control> CreateEnablePanel(const std::shared_ptr<zaf::Control>& control) {

    auto is_enabled_check_box = zaf::Create<zaf::CheckBox>();
    is_enabled_check_box->SetSize(zaf::Size(0, 20));
    is_enabled_check_box->SetText(L"Is enabled");
    is_enabled_check_box->SetIsChecked(control->IsEnabled());
    is_enabled_check_box->GetCheckStateChangeEvent().AddListener([control](const std::shared_ptr<zaf::CheckBox>& check_box) {
        control->SetIsEnabled(check_box->IsChecked());
    });

    return is_enabled_check_box;
}


static std::shared_ptr<zaf::Control> CreateVisiblePanel(const std::shared_ptr<zaf::Control>& control) {

    auto is_visible_check_box = zaf::Create<zaf::CheckBox>();
    is_visible_check_box->SetSize(zaf::Size(0, 20));
    is_visible_check_box->SetText(L"Is visible");
    is_visible_check_box->SetIsChecked(control->IsVisible());
    is_visible_check_box->GetCheckStateChangeEvent().AddListener([control](const std::shared_ptr<zaf::CheckBox>& check_box) {
        control->SetIsVisible(check_box->IsChecked());
    });

    return is_visible_check_box;
}