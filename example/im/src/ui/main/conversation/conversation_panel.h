#pragma once

#include <zaf/control/split_control.h>
#include "ui/main/content_panel.h"
#include "ui/main/conversation/conversation_avatar_manager.h"
#include "ui/main/conversation/detail/conversation_detail_view.h"
#include "ui/main/conversation/list/conversation_list_view.h"

class ConversationPanel : public zaf::SplitControl, public ContentPanel {
public:
    void Initialize() override;

    void LoadContent() override;

private:
    void InitializeLeftPane();
    void InitializeRightPane();

    void ConversationListViewSelectionChange();
    void InitializeConversationDetailView();

private:
    std::shared_ptr<ConversationAvatarManager> conversation_avatar_manager_;
    std::shared_ptr<ConversationListView> conversation_list_view_;
    std::shared_ptr<ConversationDetailView> conversation_detail_view_;

    Id current_conversation_id_ = InvalidId;
};