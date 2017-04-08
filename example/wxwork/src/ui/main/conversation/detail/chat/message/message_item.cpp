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

    auto content_size = GetContentSize();
    sender_label_->SetRect(zaf::Rect(0, TopPadding, content_size.width, SenderNameHeight));

    auto bubble_size = DeterminateContentRequiredSize(content_size.width);

    content_bubble_->SetRect(zaf::Rect(
        zaf::Point(0, TopPadding + SenderNameHeight + NameAndContentGap),
        bubble_size));
}


void MessageItem::SetMessage(const std::shared_ptr<Message>& message) {

    auto sender = Service::GetInstance().GetUser(message->sender_id);
    if (sender != nullptr) {
        sender_label_->SetText(sender->name);
    }

    content_bubble_->SetText(message->content);
}


float MessageItem::DeterminateHeight(float max_width) {

    auto content_label_size = DeterminateContentRequiredSize(max_width);
    return TopPadding + SenderNameHeight + NameAndContentGap + content_label_size.height;
}


zaf::Size MessageItem::DeterminateContentRequiredSize(float max_width) {

    zaf::Size max_content_label_size(std::min(max_width, MaxBubbleWidth), std::numeric_limits<float>::max());
    return content_bubble_->DetermineRequiredSize(max_content_label_size);
}