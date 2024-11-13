#include <zaf/control/scroll_bar_thumb.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

ZAF_OBJECT_IMPL(ScrollBarThumb);


ScrollBarThumb::ScrollBarThumb() {

}


void ScrollBarThumb::Initialize() {

    __super::Initialize();

    SetCanFocus(false);
    SetIsHorizontal(false);

    SetThumbColorPicker(ColorPicker([](const Control& control) {

        const auto& thumb = As<ScrollBarThumb>(control);

        if (thumb.IsPressed()) {
            return Color::FromRGB(0x808080);
        }

        if (thumb.IsMouseOver()) {
            return Color::FromRGB(0xA9A9A9);
        }

        return Color::FromRGB(0xCECECE);
    }));
}


void ScrollBarThumb::SetIsHorizontal(bool is_horizontal) {

    is_horizontal_ = is_horizontal;

    constexpr float padding_value = 2;
    zaf::Frame padding;
    if (is_horizontal_) {
        padding.top = padding_value;
        padding.bottom = padding_value;
    }
    else {
        padding.left = padding_value;
        padding.right = padding_value;
    }

    SetPadding(padding);
}


void ScrollBarThumb::UpdateStyle() {

    __super::UpdateStyle();

    thumb_color_field_.UpdateColor(*this);
}


void ScrollBarThumb::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    if (!IsEnabledInContext()) {
        return;
    }

    auto state_guard = canvas.PushState();

    canvas.SetBrushWithColor(ThumbColor());
    canvas.DrawRectangle(ContentRect());
}


Color ScrollBarThumb::ThumbColor() const {
    return thumb_color_field_.Color();
}


void ScrollBarThumb::SetThumbColor(const Color& color) {
    thumb_color_field_.SetColor(color, *this);
}


const ColorPicker& ScrollBarThumb::ThumbColorPicker() const {
    return thumb_color_field_.ColorPicker();
}

void ScrollBarThumb::SetThumbColorPicker(ColorPicker color_picker) {
    thumb_color_field_.SetColorPicker(std::move(color_picker), *this);
}


void ScrollBarThumb::OnMouseCaptured(const MouseCapturedInfo& event_info) {

    __super::OnMouseCaptured(event_info);

    is_dragging_ = true;

    ScrollBarThumbBeginDragInfo begin_drag_event_info{ As<ScrollBarThumb>(shared_from_this())};
    begin_drag_event_.AsObserver().OnNext(begin_drag_event_info);
}


void ScrollBarThumb::OnMouseReleased(const MouseReleasedInfo& event_info) {

    __super::OnMouseReleased(event_info);

    is_dragging_ = false;

    ScrollBarThumbEndDragInfo end_drag_event_info{ As<ScrollBarThumb>(shared_from_this()) };
    end_drag_event_.AsObserver().OnNext(end_drag_event_info);
}


void ScrollBarThumb::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);

    if (is_dragging_) {

        ScrollBarThumbDragInfo event_info{ As<ScrollBarThumb>(shared_from_this()) };
        drag_event_.AsObserver().OnNext(event_info);
    }
}

}