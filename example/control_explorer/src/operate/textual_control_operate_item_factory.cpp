#include "operate/textual_control_operate_item_factory.h"
#include <zaf/creation.h>
#include "operate/operate_item_creation.h"

void TextualControlOperateItemFactory::AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) {

    __super::AddOperateItems(items);

    items.push_back(CreateTextItem());
    items.push_back(CreateTextAlignmentPanel());
    items.push_back(CreateParagraphAlignmentPanel());
    items.push_back(CreateWordWrappingPanel());
}


std::shared_ptr<zaf::Control> TextualControlOperateItemFactory::CreateTextAlignmentPanel() {

    auto target_control =  GetTextualControl();

    return CreateOperateItemWithComboBox<zaf::TextAlignment>(
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


std::shared_ptr<zaf::Control> TextualControlOperateItemFactory::CreateParagraphAlignmentPanel() {

    auto target_control = GetTextualControl();

    return CreateOperateItemWithComboBox<zaf::ParagraphAlignment>(
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


std::shared_ptr<zaf::Control> TextualControlOperateItemFactory::CreateWordWrappingPanel() {

    auto target_control = GetTextualControl();

    return CreateOperateItemWithComboBox<zaf::WordWrapping>(
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


std::shared_ptr<zaf::Control> TextualControlOperateItemFactory::CreateTextItem() {

    auto target_control = GetTextualControl();

    auto item = CreateOperateItemWithTextBox(
        L"Text", 
        [target_control]() {
            return target_control->GetText();
        }, 
        [target_control](const std::wstring& text) {
            target_control->SetText(text);
        },
        true
    );
    item->SetHeight(80);
    return item;
}