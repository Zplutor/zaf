#include <zaf/control/scroll_bar_thumb.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(ScrollBarThumb)
ZAF_DEFINE_TYPE_PROPERTY(ThumbColor)
ZAF_DEFINE_TYPE_PROPERTY(IsHorizontal)
ZAF_DEFINE_TYPE_END

ScrollBarThumb::ScrollBarThumb() {

}


void ScrollBarThumb::Initialize() {

    __super::Initialize();

    SetCanFocused(false);
    SetIsHorizontal(false);
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


void ScrollBarThumb::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (!IsEnabledInContext()) {
        return;
    }

    auto state_guard = canvas.PushState();

    canvas.SetBrushWithColor(ThumbColor());
    canvas.DrawRectangle(ContentRect());
}


ColorPicker ScrollBarThumb::ThumbColorPicker() const {

    if (thumb_color_picker_) {
        return thumb_color_picker_;
    }
    else {

        return [](const Control& control) {

            const auto& thumb = dynamic_cast<const ScrollBarThumb&>(control);

            if (thumb.IsPressed()) {
                return Color::FromRGB(0x808080);
            }

            if (thumb.IsMouseOver()) {
                return Color::FromRGB(0xA9A9A9);
            }

            return Color::FromRGB(0xCECECE);
        };
    }
}


void ScrollBarThumb::SetThumbColorPicker(const ColorPicker& color_picker) {

    thumb_color_picker_ = color_picker;
    NeedRepaint();
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