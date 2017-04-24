#include "ui/main/conversation/detail/chat/input/message_input_view.h"
#include <zaf/window/message/keyboard_message.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/creation.h>
#include "logic/service.h"

void MessageInputView::Initialize() {
    
    __super::Initialize();

    SetLayouter(zaf::GetVerticalArrayLayouter());

    text_box_ = zaf::Create<zaf::TextBox>();
    AddChild(text_box_);

    send_button_ = zaf::Create<zaf::Button>();
    send_button_->SetText(L"Send");
    send_button_->SetCanFocused(false);
    send_button_->SetMaximumHeight(30);
    send_button_->SetMinimumHeight(30);
    send_button_->GetClickEvent().AddListener(std::bind(&MessageInputView::SendButtonClick, this));
    AddChild(send_button_);
}


void MessageInputView::SendButtonClick() {

    auto content = text_box_->GetText();
    if (content.empty()) {
        return;
    }

    Service::GetInstance().SendMessageToConversation(content, conversation_id_);

    text_box_->SetText(std::wstring());
}


bool MessageInputView::KeyDown(const zaf::KeyMessage& message) {

    if (message.GetVirtualKey() == VK_RETURN) {
        send_button_->Click();
        return true;
    }
    
    return __super::KeyDown(message);
}