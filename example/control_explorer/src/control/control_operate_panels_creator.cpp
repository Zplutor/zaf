#include "control_operate_panels_creator.h"
#include <zaf/base/string/to_numeric.h>
#include <zaf/control/button.h>
#include <zaf/control/check_box.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>

static std::shared_ptr<zaf::Control> CreateSizePanel(const std::shared_ptr<zaf::Control>& control);
static std::shared_ptr<zaf::Control> CreateEnablePanel(const std::shared_ptr<zaf::Control>& control);
static std::shared_ptr<zaf::Control> CreateVisiblePanel(const std::shared_ptr<zaf::Control>& control);

void CreateControlOperatePanels(
    const std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    operate_panels.push_back(CreateSizePanel(control));
    operate_panels.push_back(CreateEnablePanel(control));
    operate_panels.push_back(CreateVisiblePanel(control));
}


static std::shared_ptr<zaf::Control> CreateSizePanel(const std::shared_ptr<zaf::Control>& control) {

    auto panel = zaf::Create<zaf::Control>();
    panel->SetSize(zaf::Size(0, 20));
    panel->SetLayouter(zaf::GetHorizontalArrayLayouter());

    auto width_label = zaf::Create<zaf::Label>();
    width_label->SetText(L"Width");
    panel->AddChild(width_label);

    auto width_text_box = zaf::Create<zaf::TextBox>();
    width_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    width_text_box->SetText(std::to_wstring(static_cast<int>(control->GetWidth())));
    panel->AddChild(width_text_box);

    auto height_label = zaf::Create<zaf::Label>();
    height_label->SetText(L"Height");
    panel->AddChild(height_label);

    auto height_text_box = zaf::Create<zaf::TextBox>();
    height_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    height_text_box->SetText(std::to_wstring(static_cast<int>(control->GetHeight())));
    panel->AddChild(height_text_box);

    auto set_button = zaf::Create<zaf::Button>();
    set_button->SetText(L"Set");
    set_button->GetClickEvent().AddListener([width_text_box, height_text_box, control](const std::shared_ptr<zaf::ClickableControl>&){
        float width = static_cast<float>(zaf::ToNumeric<int>(width_text_box->GetText()));
        float height = static_cast<float>(zaf::ToNumeric<int>(height_text_box->GetText()));
        control->SetSize(zaf::Size(width, height));
    });
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