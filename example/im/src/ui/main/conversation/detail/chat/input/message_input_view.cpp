#include "ui/main/conversation/detail/chat/input/message_input_view.h"
#include <zaf/window/message/keyboard_message.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include "logic/service.h"

void MessageInputView::Initialize() {
    
    __super::Initialize();

    SetLayouter(zaf::Create<zaf::VerticalLayouter>());

    rich_edit_ = zaf::Create<zaf::RichEdit>();
    rich_edit_->SetBorder(zaf::Frame(0));
    rich_edit_->SetBackgroundColor(zaf::Color::Transparent());
    Subscriptions() += rich_edit_->FocusGainedEvent().Subscribe(
        std::bind(&MessageInputView::RichEditFocusChange, this));
    Subscriptions() += rich_edit_->FocusLostEvent().Subscribe(
        std::bind(&MessageInputView::RichEditFocusChange, this));
    Subscriptions() += rich_edit_->KeyDownEvent().Subscribe(
        std::bind(&MessageInputView::RichEditKeyDown, this, std::placeholders::_1));
    AddChild(rich_edit_);

    send_button_ = zaf::Create<zaf::Button>();
    send_button_->SetBorder(zaf::Frame(0));
    send_button_->SetBackgroundColor(zaf::Color::Transparent());
    /*
    send_button_->SetTextColorPicker([](const Control& control) {
        return zaf::Color::FromRGB(control.IsMouseOver() ? 0x3986E0 : 0x788894);
    });
    */
    send_button_->SetTextColor(zaf::Color::FromRGB(0x788894));
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

    RichEditFocusChange();
}


void MessageInputView::RichEditFocusChange() {
    SetBackgroundColor(zaf::Color::FromRGB(rich_edit_->IsFocused() ? 0xF3F7FA : 0xECF0F3));
}


void MessageInputView::SendButtonClick() {

    auto content = rich_edit_->Text();
    if (content.empty()) {
        return;
    }

    Service::GetInstance().SendMessageToConversation(content, conversation_id_);

    rich_edit_->SetText(std::wstring());
}


void MessageInputView::RichEditKeyDown(const zaf::KeyDownInfo& event_info) {

    if (event_info.Message().Key() == zaf::Key::Enter) {
        send_button_->Click();
        event_info.MarkAsHandled();
    }
}