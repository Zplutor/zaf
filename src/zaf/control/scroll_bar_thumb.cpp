#include <zaf/control/scroll_bar_thumb.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>

namespace zaf {

ZAF_DEFINE_TYPE(ScrollBarThumb)
ZAF_DEFINE_TYPE_END

ScrollBarThumb::ScrollBarThumb() {

}


void ScrollBarThumb::Initialize() {

    __super::Initialize();

    SetCanFocused(false);
    SetPadding(zaf::Frame{ 2, 0, 2, 0 });
}


void ScrollBarThumb::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (!IsEnabled()) {
        return;
    }

    Canvas::StateGuard state_guard(canvas);

    canvas.SetBrushWithColor(GetThumbColor());
    canvas.DrawRectangle(ContentRect());
}


ColorPicker ScrollBarThumb::GetThumbColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(property::ThumbColorPicker);
    if (color_picker != nullptr && *color_picker != nullptr) {
        return *color_picker;
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

    GetPropertyMap().SetProperty(property::ThumbColorPicker, color_picker);
    NeedRepaint();
}


void ScrollBarThumb::OnMouseCapture() {

    ClickableControl::OnMouseCapture();

    is_dragging_ = true;

    ScrollBarThumbBeginDragInfo event_info;
    event_info.scroll_bar_thumb = std::dynamic_pointer_cast<ScrollBarThumb>(shared_from_this());
    begin_drag_event_.GetObserver().OnNext(event_info);
}


void ScrollBarThumb::OnMouseRelease() {

    ClickableControl::OnMouseRelease();

    is_dragging_ = false;

    ScrollBarThumbEndDragInfo event_info;
    event_info.scroll_bar_thumb = std::dynamic_pointer_cast<ScrollBarThumb>(shared_from_this());
    end_drag_event_.GetObserver().OnNext(event_info);
}


bool ScrollBarThumb::OnMouseMove(const Point& position, const MouseMessage& message) {

    bool is_handled = __super::OnMouseMove(position, message);

    if (is_dragging_) {

        ScrollBarThumbDragInfo event_info;
        event_info.scroll_bar_thumb = std::dynamic_pointer_cast<ScrollBarThumb>(shared_from_this());
        drag_event_.GetObserver().OnNext(event_info);
    }

    return is_handled;
}

}