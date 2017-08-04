#include "manager/text_box_explore_manager.h"
#include "property/check_box_property_item.h"
#include "property/number_property_item.h"
#include "property/text_property_item.h"

std::shared_ptr<zaf::Control> TextBoxExploreManager::CreateExploredControl() {

    auto text_box = zaf::Create<zaf::TextBox>();
    text_box->SetSize(zaf::Size(200, 100));
    text_box->SetText(L"Text box");
    return text_box;
}


void TextBoxExploreManager::CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    __super::CreatePropertyItems(items);

    items.insert(items.end(), {
        CreateAllowBeepPropertyItem(),
        CreateReadOnlyPropertyItem(),
        CreateIsMultilinePropertyItem(),
        CreateMaximumLengthPropertyItem(),
        CreateUsePasswordCharacterPropertyItem(),
        CreatePasswordCharacterPropertyItem(),
    });
}


std::shared_ptr<PropertyItem> TextBoxExploreManager::CreateAllowBeepPropertyItem() {

    auto text_box = GetTextBox();
    return CreateCheckBoxPropertyItem(
        L"Allow beep",
        [text_box]() { return text_box->AllowBeep(); },
        [text_box](bool value) { text_box->SetAllowBeep(value); },
        nullptr);
}


std::shared_ptr<PropertyItem> TextBoxExploreManager::CreateReadOnlyPropertyItem() {

    auto text_box = GetTextBox();
    return CreateCheckBoxPropertyItem(
        L"Is read only",
        [text_box]() { return text_box->IsReadOnly(); },
        [text_box](bool value) { text_box->SetIsReadOnly(value); },
        nullptr);
}


std::shared_ptr<PropertyItem> TextBoxExploreManager::CreateIsMultilinePropertyItem() {

    auto text_box = GetTextBox();
    return CreateCheckBoxPropertyItem(
        L"Is mutiline",
        [text_box]() { return text_box->IsMultiline(); },
        [text_box](bool value) { text_box->SetIsMultiline(value); },
        nullptr);
}


std::shared_ptr<PropertyItem> TextBoxExploreManager::CreateMaximumLengthPropertyItem() {

    auto text_box = GetTextBox();
    return CreateFloatPropertyItem(
        L"Maximum length",
        [text_box]() { return static_cast<float>(text_box->GetMaximumLength()); },
        [text_box](float value) { text_box->SetMaximumLength(static_cast<std::uint32_t>(value)); });
}


std::shared_ptr<PropertyItem> TextBoxExploreManager::CreateUsePasswordCharacterPropertyItem() {

    auto text_box = GetTextBox();
    return CreateCheckBoxPropertyItem(
        L"Use password character",
        [text_box]() { return text_box->UsePasswordCharacter(); },
        [text_box](bool value) { text_box->SetUsePasswordCharacter(value); },
        nullptr);
}


std::shared_ptr<PropertyItem> TextBoxExploreManager::CreatePasswordCharacterPropertyItem() {

    auto text_box = GetTextBox();
    return ::CreateTextPropertyItem(
        L"Password character",
        [text_box]() { return std::wstring(1, text_box->GetPasswordCharacter()); },
        [text_box](const std::wstring& value) {
            if (!value.empty()) {
                text_box->SetPasswordCharacter(value.front());
            }
        });
}

