#include <ui/main/conversation/detail/chat/message/message_content_bubble.h>
#include <zaf/graphic/canvas.h>

void MessageContentBubble::Initialize() {

    __super::Initialize();

    SetWordWrapping(zaf::WordWrapping::Wrap);
    SetBorder(1);
    SetBorderColor(zaf::Color::Transparent);
    SetBackgroundColor(zaf::Color::Transparent);
    SetPadding(10);
}


void MessageContentBubble::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) {

    zaf::RoundedRect bubble_rect(zaf::Rect(zaf::Point(), GetSize()), 4, 4);

    canvas.SetBrushWithColor(zaf::Color::White);
    canvas.DrawRoundedRectangle(bubble_rect);

    canvas.SetBrushWithColor(zaf::Color::FromRGB(0xD0D7DF));
    canvas.DrawRoundedRectangleFrame(bubble_rect, 1);

    __super::Paint(canvas, dirty_rect);
}