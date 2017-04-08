#pragma once

#include <zaf/control/split_control.h>
#include "ui/main/conversation/detail/chat/conversation_chat_view.h"
#include "ui/main/conversation/detail/header/conversation_header_view.h"
#include "ui/main/conversation/detail/member/conversation_member_view.h"

class ConversationDetailView : public zaf::Control {
public:
    void Initialize() override;

    void SetConversation(const std::shared_ptr<Conversation>& conversation);

protected:
    void Layout(const zaf::Rect&) override;

private:
    void LayoutHeaderView();
    void LayoutMemberView();
    void LayoutChatView();

private:
    std::shared_ptr<ConversationHeaderView> header_view_;
    std::shared_ptr<ConversationMemberView> member_view_;
    std::shared_ptr<ConversationChatView> chat_view_;
};