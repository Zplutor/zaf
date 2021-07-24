#include "ui/main/conversation/detail/chat/input/message_input_view.h"
#include <zaf/window/message/keyboard_message.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include "logic/service.h"

void MessageInputView::Initialize() {
    
    __super::Initialize();

    SetLayouter(zaf::Create<zaf::VerticalLayouter>());

    text_box_ = zaf::Create<zaf::TextBox>();
    text_box_->SetBorder(zaf::Frame(0));
    text_box_->SetBackgroundColor(zaf::Color::Transparent());
    Subscriptions() += text_box_->FocusChangeEvent().Subscribe(
        std::bind(&MessageInputView::TextBoxFocusChange, this));
    AddChild(text_box_);

    send_button_ = zaf::Create<zaf::Button>();
    send_button_->SetBorder(zaf::Frame(0));
    send_button_->SetBackgroundColor(zaf::Color::Transparent());
    send_button_->SetTextColorPicker([](const Control& control) {
        return zaf::Color::FromRGB(control.IsHovered() ? 0x3986E0 : 0x788894);
    });
    send_button_->SetText(L"Send");
    send_button_->SetCanFocused(false);

    Subscriptions() += send_button_->ClickEvent().Subscribe(
        std::bind(&MessageInputView::SendButtonClick, this));

    auto send_button_container = zaf::Create<zaf::Control>();
    send_button_container->SetLayouter(zaf::Create<zaf::HorizontalLayouter>());
    send_button_container->SetBackgroundColor(zaf::Color::Transparent());
    send_button_container->SetMaxHeight(30);
    send_button_container->SetMinHeight(30);
    send_button_container->AddChild(send_button_);

    AddChild(send_button_container);

    TextBoxFocusChange();
}


void MessageInputView::TextBoxFocusChange() {
    SetBackgroundColor(zaf::Color::FromRGB(text_box_->IsFocused() ? 0xF3F7FA : 0xECF0F3));
}


void MessageInputView::SendButtonClick() {

    auto content = text_box_->Text();
    if (content.empty()) {
        return;
    }

    Service::GetInstance().SendMessageToConversation(content, conversation_id_);

    text_box_->SetText(std::wstring());
}


bool MessageInputView::OnKeyDown(const zaf::KeyMessage& message) {

    if (message.GetVirtualKey() == VK_RETURN) {
        send_button_->Click();
        return true;
    }
    
    return __super::OnKeyDown(message);
}