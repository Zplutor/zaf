#pragma once

#include <zaf/control/split_control.h>
#include "ui/main/conversation/detail/chat/input/message_input_view.h"
#include "ui/main/conversation/detail/chat/message/conversation_message_list_view.h"
#include "entity/conversation.h"

class ConversationChatView : public zaf::SplitControl {
public:
    void Initialize() override;

    void SetConversation(const std::shared_ptr<Conversation>& conversation);

private:
    std::shared_ptr<ConversationMessageListView> message_list_view_;
    std::shared_ptr<MessageInputView> message_input_view_;
};