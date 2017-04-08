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
    zaf::Size DeterminateContentRequiredSize(float max_width);

private:
    std::shared_ptr<zaf::Label> sender_label_;
    std::shared_ptr<MessageContentBubble> content_bubble_;
};