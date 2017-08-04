#include "manager/textual_control_explore_manager.h"
#include "property/combo_box_property_item.h"
#include "property/text_property_item.h"

void TextualControlExploreManager::CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) {

    __super::CreatePropertyItems(items);

    items.insert(items.end(), {
        CreateTextAlignmentPropertyItem(),
        CreateParagraphAlignmentPropertyItem(),
        CreateWordWrappingPropertyItem(),
        CreateTextPropertyItem(),
    });
}


std::shared_ptr<PropertyItem> TextualControlExploreManager::CreateTextAlignmentPropertyItem() {

    auto target_control = GetTextualControl();

    return CreateComboBoxPropertyItem<zaf::TextAlignment>(
        L"Text alignment",
        {
            { L"Leading", zaf::TextAlignment::Leading, },
            { L"Center", zaf::TextAlignment::Center, },
            { L"Tailing", zaf::TextAlignment::Tailing, },
            { L"Justified", zaf::TextAlignment::Justified, }
        },
        [target_control]() {
            return target_control->GetTextAlignment();
        },
        [target_control](zaf::TextAlignment text_alignment) {
            target_control->SetTextAlignment(text_alignment);
        },
        nullptr
    );
}


std::shared_ptr<PropertyItem> TextualControlExploreManager::CreateParagraphAlignmentPropertyItem() {

    auto target_control = GetTextualControl();

    return CreateComboBoxPropertyItem<zaf::ParagraphAlignment>(
        L"Paragraph alignment",
        {
            { L"Near", zaf::ParagraphAlignment::Near },
            { L"Center", zaf::ParagraphAlignment::Center },
            { L"Far", zaf::ParagraphAlignment::Far }
        },
        [target_control]() { 
            return target_control->GetParagraphAlignment(); 
        },
        [target_control](zaf::ParagraphAlignment value) { 
            target_control->SetParagraphAlignment(value);
        },
        nullptr
    );
}


std::shared_ptr<PropertyItem> TextualControlExploreManager::CreateWordWrappingPropertyItem() {

    auto target_control = GetTextualControl();

    return CreateComboBoxPropertyItem<zaf::WordWrapping>(
        L"Word wrapping",
        {
            { L"No wrap", zaf::WordWrapping::NoWrap },
            { L"Wrap", zaf::WordWrapping::Wrap },
            { L"Whole word", zaf::WordWrapping::WholeWord },
            { L"Character", zaf::WordWrapping::Character },
            { L"Emergency break", zaf::WordWrapping::EmergencyBreak },
        },
        [target_control]() { return target_control->GetWordWrapping(); },
        [target_control](zaf::WordWrapping value) { target_control->SetWordWrapping(value); },
        nullptr
    );
}


std::shared_ptr<PropertyItem> TextualControlExploreManager::CreateTextPropertyItem() {

    auto target_control = GetTextualControl();

    auto item = CreateMultilineTextPropertyItem(
        L"Text",
        [target_control]() {
            return target_control->GetText();
        },
        [target_control](const std::wstring& text) {
            target_control->SetText(text);
        }
    );
    item->SetHeight(80);
    return item;
}