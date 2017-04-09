#include "ui/main/conversation/detail/chat/message/message_item.h"
#include <algorithm>
#include "logic/service.h"

#undef min

static float TopPadding = 15;
static float SenderNameHeight = 16;
static float NameAndContentGap = 5;

static float MaxBubbleWidth = 400;

void MessageItem::Initialize() {

    __super::Initialize();

    sender_label_ = zaf::Create<zaf::Label>();
    sender_label_->SetDefaultTextColor(zaf::Color::FromRGB(0x49709B));
    AddChild(sender_label_);

    content_bubble_ = zaf::Create<MessageContentBubble>();
    AddChild(content_bubble_);
}


void MessageItem::Layout(const zaf::Rect& previous_rect) {

    float top = TopPadding;

    auto content_size = GetContentSize();
    if (sender_label_->IsVisible()) {
        sender_label_->SetRect(zaf::Rect(0, top, content_size.width, SenderNameHeight));
        top += SenderNameHeight + NameAndContentGap;
    }

    auto bubble_size = DeterminateContentRequiredSize(content_size.width);

    zaf::Point bubble_position;
    bubble_position.x = is_current_user_ ? content_size.width - bubble_size.width : 0;
    bubble_position.y = top;

    content_bubble_->SetRect(zaf::Rect(bubble_position, bubble_size));
}


void MessageItem::SetMessage(const std::shared_ptr<Message>& message) {

    is_current_user_ = message->sender_id == Service::GetInstance().GetCurrentUserId();
    
    auto conversation = Service::GetInstance().GetConversation(message->conversation_id);
    if (conversation != nullptr) {
        is_in_group_chat_ = conversation->members.size() > 2;
    }

    bool show_sender_name = !is_current_user_ && is_in_group_chat_;
    sender_label_->SetIsVisible(show_sender_name);

    if (show_sender_name) {
        auto sender = Service::GetInstance().GetUser(message->sender_id);
        if (sender != nullptr) {
            sender_label_->SetText(sender->name);
        }
    }

    content_bubble_->SetStyle(is_current_user_ ? MessageContentBubble::Style::Highlight : MessageContentBubble::Style::Normal);
    content_bubble_->SetText(message->content);
}


float MessageItem::DeterminateHeight(float max_width) {

    float height = TopPadding;

    auto content_label_size = DeterminateContentRequiredSize(max_width);
    height += content_label_size.height;

    if (sender_label_->IsVisible()) {
        height += SenderNameHeight + NameAndContentGap;
    }

    return height;
}


zaf::Size MessageItem::DeterminateContentRequiredSize(float max_width) {

    zaf::Size max_content_label_size(std::min(max_width, MaxBubbleWidth), std::numeric_limits<float>::max());
    return content_bubble_->DetermineRequiredSize(max_content_label_size);
}