#include "operate/text_box_operate_item_factory.h"
#include "operate_item_creation.h"

void TextBoxOperateItemFactory::AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) {

    __super::AddOperateItems(items);

    items.push_back(CreateAllowBeepOperateItem());
    items.push_back(CreateReadOnlyOperateItem());
    items.push_back(CreateIsMultilineOperateItem());
    items.push_back(CreateMaximumLengthOperateItem());
    items.push_back(CreateUsePasswordCharacterOperateItem());
    items.push_back(CreatePasswordCharacterOperateItem());
}


std::shared_ptr<zaf::Control> TextBoxOperateItemFactory::CreateAllowBeepOperateItem() {

    auto text_box = GetTextBox();
    return CreateOperateItemWithCheckBox(
        L"Allow beep",
        [text_box]() { return text_box->AllowBeep(); },
        [text_box](bool value) { text_box->SetAllowBeep(value); },
        nullptr);
}


std::shared_ptr<zaf::Control> TextBoxOperateItemFactory::CreateReadOnlyOperateItem() {

    auto text_box = GetTextBox();
    return CreateOperateItemWithCheckBox(
        L"Is read only",
        [text_box]() { return text_box->IsReadOnly(); },
        [text_box](bool value) { text_box->SetIsReadOnly(value); },
        nullptr);
}


std::shared_ptr<zaf::Control> TextBoxOperateItemFactory::CreateIsMultilineOperateItem() {

    auto text_box = GetTextBox();
    return CreateOperateItemWithCheckBox(
        L"Is mutiline",
        [text_box]() { return text_box->IsMultiline(); },
        [text_box](bool value) { text_box->SetIsMultiline(value); },
        nullptr);
}


std::shared_ptr<zaf::Control> TextBoxOperateItemFactory::CreateMaximumLengthOperateItem() {

    auto text_box = GetTextBox();
    return CreateOperateItemWithNumericTextBox(
        L"Maximum length",
        [text_box]() { return static_cast<float>(text_box->GetMaximumLength()); },
        [text_box](float value) { text_box->SetMaximumLength(static_cast<std::uint32_t>(value)); });
}


std::shared_ptr<zaf::Control> TextBoxOperateItemFactory::CreateUsePasswordCharacterOperateItem() {

    auto text_box = GetTextBox();
    return CreateOperateItemWithCheckBox(
        L"Use password character",
        [text_box]() { return text_box->UsePasswordCharacter(); },
        [text_box](bool value) { text_box->SetUsePasswordCharacter(value); },
        nullptr);
}


std::shared_ptr<zaf::Control> TextBoxOperateItemFactory::CreatePasswordCharacterOperateItem() {

    auto text_box = GetTextBox();
    return CreateOperateItemWithTextBox(
        L"Password character",
        [text_box]() { return std::wstring(1, text_box->GetPasswordCharacter()); },
        [text_box](const std::wstring& value) { 
            if (! value.empty()) {
                text_box->SetPasswordCharacter(value.front());
            }
        },
        nullptr);
}