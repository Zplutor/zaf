#pragma once

#include <zaf/control/split_control.h>
#include "ui/main/content_panel.h"
#include "ui/main/conversation/conversation_avatar_manager.h"
#include "ui/main/conversation/list/conversation_list_view.h"

class ConversationPanel : public zaf::SplitControl, public ContentPanel {
public:
    void Initialize() override;

    void LoadContent() override;

private:
    void InitializeLeftPane();

private:
    std::shared_ptr<ConversationAvatarManager> conversation_avatar_manager_;
    std::shared_ptr<ConversationListView> conversation_list_view_;
};