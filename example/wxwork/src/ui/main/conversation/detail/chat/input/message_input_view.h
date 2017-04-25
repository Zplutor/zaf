#pragma once

#include <zaf/control/button.h>
#include <zaf/control/control.h>
#include <zaf/control/text_box.h>
#include "entity/conversation.h"

class MessageInputView : public zaf::Control {
public:
    void Initialize() override;

    void SetConversation(const std::shared_ptr<Conversation>& conversation) {
        conversation_id_ = conversation->id;
    }

protected:
    bool KeyDown(const zaf::KeyMessage& message) override;

private:
    void TextBoxFocusChange();
    void SendButtonClick();

private:
    std::shared_ptr<zaf::TextBox> text_box_;
    std::shared_ptr<zaf::Button> send_button_;

    Id conversation_id_ = InvalidId;
};