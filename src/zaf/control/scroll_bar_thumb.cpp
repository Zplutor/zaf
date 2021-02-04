#include <zaf/control/scroll_bar_thumb.h>
#include <zaf/graphic/canvas.h>
#include <zaf/parsing/parsers/scroll_bar_thumb_parser.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/serialization/properties.h>

namespace zaf {

ZAF_DEFINE_REFLECTION_TYPE(ScrollBarThumb)
    ZAF_DEFINE_PARSER(ScrollBarThumbParser)
ZAF_DEFINE_END

ScrollBarThumb::ScrollBarThumb() :
    is_dragging_(false) {

}


void ScrollBarThumb::Initialize() {

    __super::Initialize();

    SetCanFocused(false);
}


void ScrollBarThumb::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (!IsEnabled()) {
        return;
    }

    Canvas::StateGuard state_guard(canvas);

    auto thumb_rect = GetContentRect();
    const float spacing = 2;
    if (IsHorizontal()) {
        thumb_rect.Inflate(0, -spacing);
    }
    else {
        thumb_rect.Inflate(-spacing, 0);
    }

    canvas.SetBrushWithColor(GetThumbColor());
    canvas.DrawRectangle(thumb_rect);
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

            if (thumb.IsHovered()) {
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

    bool result = ClickableControl::OnMouseMove(position, message);

    if (is_dragging_) {

        ScrollBarThumbDragInfo event_info;
        event_info.scroll_bar_thumb = std::dynamic_pointer_cast<ScrollBarThumb>(shared_from_this());
        drag_event_.GetObserver().OnNext(event_info);
    }

    return result;
}

}