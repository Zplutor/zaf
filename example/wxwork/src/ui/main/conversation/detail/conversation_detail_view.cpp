#include "ui/main/conversation/detail/conversation_detail_view.h"

void ConversationDetailView::Initialize() {

    __super::Initialize();

    member_view_ = zaf::Create<ConversationMemberView>();
    AddChild(member_view_);
}


void ConversationDetailView::Layout(const zaf::Rect&) {

    LayoutMemberView();
}


void ConversationDetailView::LayoutMemberView() {

    auto content_rect = GetContentRect();
    const float width = 120;
    member_view_->SetRect(zaf::Rect(
        content_rect.size.width - width, 
        0,
        width, 
        content_rect.size.height));
}


void ConversationDetailView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    member_view_->SetConversation(conversation);
}