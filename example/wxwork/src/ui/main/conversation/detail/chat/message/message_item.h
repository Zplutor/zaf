#pragma once

#include <zaf/control/label.h>
#include <zaf/control/list_control.h>
#include "entity/message.h"
#include "ui/main/conversation/detail/chat/message/message_content_bubble.h"

class MessageItem : public zaf::ListControl::Item {
public:
    void Initialize() override;

    void SetMessage(const std::shared_ptr<Message>& message);

    float DeterminateHeight(float max_width);

protected:
    void Layout(const zaf::Rect& previous_rect) override;

private:
    std::shared_ptr<zaf::Label> sender_label_;
    std::shared_ptr<MessageContentBubble> content_bubble_;
    bool is_current_user_ = false;
    bool is_in_group_chat_ = false;
};