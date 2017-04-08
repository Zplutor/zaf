#include "ui/main/conversation/detail/chat/conversation_chat_view.h"
#include <zaf/creation.h>

void ConversationChatView::Initialize() {

    __super::Initialize();

    SetIsHorizontal(true);

    message_list_view_ = zaf::Create<ConversationMessageListView>();
    SetFirstPane(message_list_view_);
}


void ConversationChatView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    message_list_view_->SetConversation(conversation);
}