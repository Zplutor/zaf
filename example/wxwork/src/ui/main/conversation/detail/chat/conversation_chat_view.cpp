#include "ui/main/conversation/detail/chat/conversation_chat_view.h"
#include <zaf/creation.h>

void ConversationChatView::Initialize() {

    __super::Initialize();

    SetIsHorizontalSplit(true);
    SetIsSplitBarDistanceFlipped(true);
    SetSplitBarDistance(150);
    SetMinSplitBarDistance(150);
    SetMaxSplitBarDistance(350);
    GetSplitBar()->SetSplitterColor(zaf::Color::FromRGB(0xD5DDE7));

    message_list_view_ = zaf::Create<ConversationMessageListView>();
    SetFirstPane(message_list_view_);

    message_input_view_ = zaf::Create<MessageInputView>();
    SetSecondPane(message_input_view_);
}


void ConversationChatView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    message_list_view_->SetConversation(conversation);
    message_input_view_->SetConversation(conversation);
}