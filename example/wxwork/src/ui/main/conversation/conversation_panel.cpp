#include "ui/main/conversation/conversation_panel.h"
#include <zaf/control/layout/linear_layouter.h>

void ConversationPanel::Initialize() {

    __super::Initialize();

    conversation_avatar_manager_ = std::make_shared<ConversationAvatarManager>();

    SetIsHorizontalSplit(false);
    SetSplitBarDistance(250);
    SetMinimumSplitBarDistance(250);
    SetMaximumSplitBarDistance(300);
    GetSplitBar()->SetSplitterColor(zaf::Color::FromRGB(0xD5DDE7));

    InitializeLeftPane();
    InitializeRightPane();
}


void ConversationPanel::InitializeLeftPane() {

    const auto& left_pane = GetFirstPane();
    left_pane->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

    conversation_list_view_ = zaf::Create<ConversationListView>();
    conversation_list_view_->SetConversationAvatarManager(conversation_avatar_manager_);

    Subscriptions() += conversation_list_view_->SelectionChangeEvent().Subscribe(
        std::bind(&ConversationPanel::ConversationListViewSelectionChange, this));

    left_pane->AddChild(conversation_list_view_);
}


void ConversationPanel::InitializeRightPane() {

    const auto& right_panel = GetSecondPane();
    right_panel->SetLayouter(zaf::Create<zaf::HorizontalLayouter>());
}


void ConversationPanel::LoadContent() {

    conversation_list_view_->LoadConversations();
}


void ConversationPanel::ConversationListViewSelectionChange() {

    InitializeConversationDetailView();

    auto selected_conversation = conversation_list_view_->GetSelectedConversation();
    if (selected_conversation == nullptr) {
        return;
    }

    if (selected_conversation->id == current_conversation_id_) {
        return;
    }

    current_conversation_id_ = selected_conversation->id;
    conversation_detail_view_->SetConversation(selected_conversation);
}


void ConversationPanel::InitializeConversationDetailView() {

    if (conversation_detail_view_ != nullptr) {
        return;
    }

    conversation_detail_view_ = zaf::Create<ConversationDetailView>();
    GetSecondPane()->AddChild(conversation_detail_view_);
}