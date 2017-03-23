#pragma once

#include <zaf/control/list_control.h>
#include "ui/main/conversation/conversation_avatar_manager.h"

class ConversationListView : public zaf::ListControl {
public:
    void Initialize() override;

    void LoadConversations(const std::shared_ptr<ConversationAvatarManager>& avatra_manager);
};