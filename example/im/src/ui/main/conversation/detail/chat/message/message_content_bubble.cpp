#include <ui/main/conversation/detail/chat/message/message_content_bubble.h>
#include <zaf/graphic/canvas/canvas.h>

void MessageContentBubble::Initialize() {

    __super::Initialize();

    SetWordWrapping(zaf::dwrite::WordWrapping::Wrap);
    SetBorder(zaf::Frame(1));
    SetBorderColor(zaf::Color::Transparent());
    SetBackgroundColor(zaf::Color::Transparent());
    SetPadding(zaf::Frame(10));
}


void MessageContentBubble::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) const {

    zaf::RoundedRect bubble_rect(zaf::Rect(zaf::Point(), Size()), 4, 4);
    auto state_guard = canvas.PushState();

    auto background_color = style_ == Style::Highlight ? zaf::Color::FromRGB(0xBFDCFF) : zaf::Color::White();
    state_guard.SetBrush(background_color);
    canvas.FillRoundedRectangle(bubble_rect);

    auto border_color = style_ == Style::Highlight ? zaf::Color::FromRGB(0xA6C8EE) : zaf::Color::FromRGB(0xD0D7DF);
    canvas.DrawRoundedRectangle(bubble_rect, border_color, 1);

    __super::Paint(canvas, dirty_rect);
}