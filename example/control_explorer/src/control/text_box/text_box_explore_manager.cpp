#include "control/text_box/text_box_explore_manager.h"
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/control/button.h>
#include <zaf/control/check_box.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include "control/common.h"
#include "control/control_operate_panels_creator.h"
#include "control/textual_control_operate_panels_creator.h"

static std::shared_ptr<zaf::Control> CreateInsetOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box);
static std::shared_ptr<zaf::Control> CreateAllowBeepOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box);
static std::shared_ptr<zaf::Control> CreateReadOnlyOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box);
static std::shared_ptr<zaf::Control> CreateMultilineOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box);
static std::shared_ptr<zaf::Control> CreateMaxLengthOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box);
static std::shared_ptr<zaf::Control> CreatePasswordCharacterOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box);
static std::shared_ptr<zaf::Control> CreateSelectionRangeOperatePanle(const std::shared_ptr<zaf::TextBox>& text_box);

void TextBoxExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    auto text_box = zaf::Create<zaf::TextBox>();
    text_box->SetSize(zaf::Size(200, 100));
    text_box->SetText(L"Text box");

    control = text_box;

    CreateControlOperatePanels(text_box, operate_panels);
    CreateTextualControlOperatePanels(text_box, operate_panels);
    operate_panels.push_back(CreateInsetOperatePanel(text_box));
    operate_panels.push_back(CreateAllowBeepOperatePanel(text_box));
    operate_panels.push_back(CreateReadOnlyOperatePanel(text_box));
    operate_panels.push_back(CreateMultilineOperatePanel(text_box));
    operate_panels.push_back(CreateMaxLengthOperatePanel(text_box));
    operate_panels.push_back(CreatePasswordCharacterOperatePanel(text_box));
    operate_panels.push_back(CreateSelectionRangeOperatePanle(text_box));
}


static std::shared_ptr<zaf::Control> CreateInsetOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box) {

    auto panel = CreateOperateContainerPanel(1);

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Inset");
    panel->AddChild(label);

    //Left
    auto left_label = zaf::Create<zaf::Label>();
    left_label->SetText(L"Left");
    panel->AddChild(left_label);

    auto left_text_box = zaf::Create<zaf::TextBox>();
    left_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    panel->AddChild(left_text_box);

    //Top
    auto top_label = zaf::Create<zaf::Label>();
    top_label->SetText(L"Top");
    panel->AddChild(top_label);

    auto top_text_box = zaf::Create<zaf::TextBox>();
    top_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    panel->AddChild(top_text_box);

    //Right
    auto right_label = zaf::Create<zaf::Label>();
    right_label->SetText(L"Right");
    panel->AddChild(right_label);

    auto right_text_box = zaf::Create<zaf::TextBox>();
    right_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    panel->AddChild(right_text_box);

    //Bottom
    auto bottom_label = zaf::Create<zaf::Label>();
    bottom_label->SetText(L"Bottom");
    panel->AddChild(bottom_label);

    auto bottom_text_box = zaf::Create<zaf::TextBox>();
    bottom_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    panel->AddChild(bottom_text_box);

    auto set_button = zaf::Create<zaf::Button>();
    set_button->SetText(L"Set");
    set_button->GetClickEvent().AddListener(std::bind([left_text_box, top_text_box, right_text_box, bottom_text_box, text_box]() {

        int left = zaf::ToNumeric<int>(left_text_box->GetText());
        int top = zaf::ToNumeric<int>(top_text_box->GetText());
        int right = zaf::ToNumeric<int>(right_text_box->GetText());
        int bottom = zaf::ToNumeric<int>(bottom_text_box->GetText());
        text_box->SetInset(zaf::Frame(
            static_cast<float>(left),
            static_cast<float>(top),
            static_cast<float>(right),
            static_cast<float>(bottom)));
    }));
    panel->AddChild(set_button);

    auto frame = text_box->GetInset();
    left_text_box->SetText(zaf::ToWideString(static_cast<int>(frame.left)));
    top_text_box->SetText(zaf::ToWideString(static_cast<int>(frame.top)));
    right_text_box->SetText(zaf::ToWideString(static_cast<int>(frame.right)));
    bottom_text_box->SetText(zaf::ToWideString(static_cast<int>(frame.bottom)));

    return panel;
}


static std::shared_ptr<zaf::Control> CreateAllowBeepOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box) {

    auto panel = CreateOperateContainerPanel(1);

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetText(L"Allow beep");
    check_box->SetIsChecked(text_box->AllowBeep());
    check_box->GetCheckStateChangeEvent().AddListener([text_box](const std::shared_ptr<zaf::CheckBox>& check_box) {
        text_box->SetAllowBeep(check_box->IsChecked());
    });
    panel->AddChild(check_box);

    return panel;
}


static std::shared_ptr<zaf::Control> CreateReadOnlyOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box) {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    check_box->SetText(L"Is readonly");
    check_box->SetIsChecked(text_box->IsReadOnly());
    check_box->GetCheckStateChangeEvent().AddListener([text_box](const std::shared_ptr<zaf::CheckBox>& check_box) {
        text_box->SetIsReadOnly(check_box->IsChecked());
    });
    return check_box;
}


static std::shared_ptr<zaf::Control> CreateMultilineOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box) {

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    check_box->SetText(L"Is multiline");
    check_box->SetIsChecked(text_box->IsMultiline());
    check_box->GetCheckStateChangeEvent().AddListener([text_box](const std::shared_ptr<zaf::CheckBox>& check_box) {
        text_box->SetIsMultiline(check_box->IsChecked());
    });
    return check_box;
}


static std::shared_ptr<zaf::Control> CreateMaxLengthOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box) {

    auto panel = CreateOperateContainerPanel(1);

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Max length");
    panel->AddChild(label);

    auto max_length_text_box = zaf::Create<zaf::TextBox>();
    max_length_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    max_length_text_box->SetText(std::to_wstring(text_box->GetMaximumLength()));
    panel->AddChild(max_length_text_box);

    auto set_button = zaf::Create<zaf::Button>();
    set_button->SetText(L"Set");
    set_button->GetClickEvent().AddListener([max_length_text_box, text_box](const std::shared_ptr<zaf::ClickableControl>&) {
        auto max_length = zaf::ToNumeric<std::uint32_t>(max_length_text_box->GetText());
        text_box->SetMaximumLength(max_length);
    });
    panel->AddChild(set_button);

    return panel;
}


static std::shared_ptr<zaf::Control> CreatePasswordCharacterOperatePanel(const std::shared_ptr<zaf::TextBox>& text_box) {

    auto panel = CreateOperateContainerPanel(1);

    auto check_box = zaf::Create<zaf::CheckBox>();
    check_box->SetText(L"Use password character");
    check_box->SetIsChecked(text_box->UsePasswordCharacter());
    check_box->GetCheckStateChangeEvent().AddListener([text_box](const std::shared_ptr<zaf::CheckBox>& check_box) {
        text_box->SetUsePasswordCharacter(check_box->IsChecked());
    });
    panel->AddChild(check_box);

    auto char_text_box = zaf::Create<zaf::TextBox>();
    char_text_box->SetMaximumLength(1);
    char_text_box->SetText(std::wstring(1, text_box->GetPasswordCharacter()));
    panel->AddChild(char_text_box);

    auto set_button = zaf::Create<zaf::Button>();
    set_button->SetText(L"Set");
    set_button->GetClickEvent().AddListener(std::bind([char_text_box, text_box]() {
        auto char_string = char_text_box->GetText();
        if (! char_string.empty()) {
            text_box->SetPasswordCharacter(char_string[0]);
        }
    }));
    panel->AddChild(set_button);

    return panel;
}


static std::shared_ptr<zaf::Control> CreateSelectionRangeOperatePanle(const std::shared_ptr<zaf::TextBox>& text_box) {

    auto panel = CreateOperateContainerPanel(1);

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Selection range");
    panel->AddChild(label);

    auto index_label = zaf::Create<zaf::Label>();
    index_label->SetText(L"Index");
    panel->AddChild(index_label);

    auto index_text_box = zaf::Create<zaf::TextBox>();
    index_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    panel->AddChild(index_text_box);

    auto length_label = zaf::Create<zaf::Label>();
    length_label->SetText(L"Length");
    panel->AddChild(length_label);

    auto length_text_box = zaf::Create<zaf::TextBox>();
    length_text_box->SetTextValidator(zaf::GetNumberTextValidator());
    panel->AddChild(length_text_box);

    auto set_button = zaf::Create<zaf::Button>();
    set_button->SetText(L"Set");
    set_button->GetClickEvent().AddListener(std::bind([index_text_box, length_text_box, text_box]() {

        auto index = zaf::ToNumeric<std::size_t>(index_text_box->GetText());
        auto length = zaf::ToNumeric<std::size_t>(length_text_box->GetText());
        text_box->SetSelectionRange(zaf::TextRange(index, length));
    }));
    panel->AddChild(set_button);

    auto update_text_boxes = [index_text_box, length_text_box](const std::shared_ptr<zaf::TextBox>& text_box) {
    
        auto selection_range = text_box->GetSelectionRange();
        index_text_box->SetText(zaf::ToWideString(selection_range.index));
        length_text_box->SetText(zaf::ToWideString(selection_range.length));
    };

    update_text_boxes(text_box);

    text_box->GetSelectionChangeEvent().AddListener([update_text_boxes](const std::shared_ptr<zaf::TextBox>& text_box) {
        update_text_boxes(text_box);
    });

    return panel;
}

