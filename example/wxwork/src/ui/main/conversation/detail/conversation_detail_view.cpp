#include "ui/main/conversation/detail/conversation_detail_view.h"

void ConversationDetailView::Initialize() {

    __super::Initialize();

    member_view_ = zaf::Create<ConversationMemberView>();
    AddChild(member_view_);

    header_view_ = zaf::Create<ConversationHeaderView>();
    AddChild(header_view_);

    chat_view_ = zaf::Create<ConversationChatView>();
    AddChild(chat_view_);
}


void ConversationDetailView::Layout(const zaf::Rect&) {

    LayoutHeaderView();
    LayoutMemberView();
    LayoutChatView();
}


void ConversationDetailView::LayoutHeaderView() {

    const float height = 50;
    auto content_size = GetContentSize();
    header_view_->SetRect(zaf::Rect(0, 0, content_size.width, height));
}


void ConversationDetailView::LayoutMemberView() {

    const float width = 120;
    auto content_size = GetContentSize();
    float header_height = header_view_->GetHeight();
    member_view_->SetRect(zaf::Rect(
        content_size.width - width, 
        header_height,
        width, 
        content_size.height - header_height));
}


void ConversationDetailView::LayoutChatView() {

    float header_view_height = header_view_->GetHeight();
    float member_view_width = member_view_->IsVisible() ? member_view_->GetWidth() : 0;
    auto content_size = GetContentSize();
    chat_view_->SetRect(zaf::Rect(
        0,
        header_view_height, 
        content_size.width - member_view_width, 
        content_size.height - header_view_height));
}


void ConversationDetailView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    header_view_->SetConversation(conversation);
    chat_view_->SetConversation(conversation);

    if (conversation->members.size() > 2) {
        member_view_->SetIsVisible(true);
        member_view_->SetConversation(conversation);
    }
    else {
        member_view_->SetIsVisible(false);
    }

    NeedRelayout();
}