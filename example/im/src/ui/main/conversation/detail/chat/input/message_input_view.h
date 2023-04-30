#pragma once

#include <zaf/control/button.h>
#include <zaf/control/control.h>
#include <zaf/control/rich_edit.h>
#include "entity/conversation.h"

class MessageInputView : public zaf::Control {
public:
    void Initialize() override;

    void SetConversation(const std::shared_ptr<Conversation>& conversation) {
        conversation_id_ = conversation->id;
    }

private:
    void RichEditFocusChange();
    void SendButtonClick();
    void RichEditKeyDown(const zaf::KeyDownInfo& event_info);

private:
    std::shared_ptr<zaf::RichEdit> rich_edit_;
    std::shared_ptr<zaf::Button> send_button_;

    Id conversation_id_ = InvalidId;
};