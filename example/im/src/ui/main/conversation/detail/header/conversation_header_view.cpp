#include "ui/main/conversation/detail/header/conversation_header_view.h"
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/creation.h>
#include "ui/main/conversation/common/title_generating.h"

void ConversationHeaderView::Initialize() {

    __super::Initialize();

    SetPadding(zaf::Frame(12, 0, 12, 0));
    SetBorder(zaf::Frame(0, 0, 0, 1));
    SetBorderColor(zaf::Color::FromRGB(0xD5DDE7));
    SetBackgroundColor(zaf::Color::FromRGB(0xECF0F3));
    SetLayouter(zaf::Create<zaf::HorizontalLayouter>());

    title_label_ = zaf::Create<zaf::Label>();
    title_label_->SetTextAlignment(zaf::dwrite::TextAlignment::Leading);
    title_label_->SetParagraphAlignment(zaf::dwrite::ParagraphAlignment::Center);
    title_label_->SetTextTrimming(zaf::dwrite::TextTrimmingGranularity::Character);
    AddChild(title_label_);
}


void ConversationHeaderView::SetConversation(const std::shared_ptr<Conversation>& conversation) {

    title_label_->SetText(GenerateConversationTitle(conversation));
}