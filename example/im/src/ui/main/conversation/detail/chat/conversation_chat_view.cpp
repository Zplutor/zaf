#include "ui/main/conversation/detail/chat/conversation_chat_view.h"
#include <zaf/creation.h>

void ConversationChatView::Initialize() {

    __super::Initialize();

    SetIsHorizontalSplit(true);
    SetSplitDistance(350);
    SetSecondPaneMinLength(150);
    SetSecondPaneMaxLength(350);
    SplitBar()->SetSplitterColor(zaf::Color::FromRGB(0xD5DDE7));

    message_list_view_ = zaf::Create<ConversationMessageListView>();
    SetFirstPane(message_list_view_);

    message_input_view_ = zaf::Create<MessageInputView>();
    SetSecondPane(message_input_view_);
}


void ConversationChatView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    message_list_view_->SetConversation(conversation);
    message_input_view_->SetConversation(conversation);
}