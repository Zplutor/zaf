#include "ui/main/conversation/conversation_panel.h"
#include <zaf/control/layout/array_layouter.h>

void ConversationPanel::Initialize() {

    __super::Initialize();

    SetIsHorizontal(false);

    InitializeLeftPane();
}


void ConversationPanel::InitializeLeftPane() {

    const auto& left_pane = GetFirstPane();
    left_pane->SetLayouter(zaf::GetVerticalArrayLayouter());

    conversation_list_view_ = zaf::Create<ConversationListView>();
    left_pane->AddChild(conversation_list_view_);
}


void ConversationPanel::LoadContent() {

    conversation_avatar_manager_ = std::make_shared<ConversationAvatarManager>();
    conversation_list_view_->LoadConversations(conversation_avatar_manager_);
}