#include <ui/main/conversation/detail/chat/message/message_content_bubble.h>
#include <zaf/graphic/canvas.h>

void MessageContentBubble::Initialize() {

    __super::Initialize();

    SetWordWrapping(zaf::WordWrapping::Wrap);
    SetBorder(zaf::Frame(1));
    SetBorderColor(zaf::Color::Transparent());
    SetBackgroundColor(zaf::Color::Transparent());
    SetPadding(zaf::Frame(10));
}


void MessageContentBubble::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) {

    zaf::RoundedRect bubble_rect(zaf::Rect(zaf::Point(), Size()), 4, 4);

    auto background_color = style_ == Style::Highlight ? zaf::Color::FromRGB(0xBFDCFF) : zaf::Color::White();
    canvas.SetBrushWithColor(background_color);
    canvas.DrawRoundedRectangle(bubble_rect);

    auto border_color = style_ == Style::Highlight ? zaf::Color::FromRGB(0xA6C8EE) : zaf::Color::FromRGB(0xD0D7DF);
    canvas.SetBrushWithColor(border_color);
    canvas.DrawRoundedRectangleFrame(bubble_rect, 1);

    __super::Paint(canvas, dirty_rect);
}