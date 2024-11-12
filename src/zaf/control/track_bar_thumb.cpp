#include <zaf/control/track_bar_thumb.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void TrackBarThumb::Initialize() {

    __super::Initialize();

    SetCanFocused(false);

    SetThumbColorPicker(ColorPicker([](const Control& control) {

        const auto& thumb = As<TrackBarThumb>(control);

        if (thumb.IsDragging()) {
            return Color::FromRGB(0x808080);
        }

        if (thumb.IsMouseOver()) {
            return Color::FromRGB(0xA9A9A9);
        }

        return Color::FromRGB(0xCECECE);
    }));
}


void TrackBarThumb::UpdateStyle() {

    __super::UpdateStyle();

    thumb_color_field_.UpdateColor(*this);
}


void TrackBarThumb::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    canvas.DrawRectangle(ContentRect(), ThumbColor());
}


void TrackBarThumb::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() != MouseButton::Left) {
        return;
    }

    CaptureMouse();
    is_dragging_ = true;

    drag_started_event_.Raise(TrackBarThumbDragStartedInfo{ 
        As<TrackBarThumb>(shared_from_this()) 
    });

    event_info.MarkAsHandled();
}


void TrackBarThumb::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    if (!is_dragging_) {
        return;
    }

    dragging_event_.Raise(TrackBarThumbDraggingInfo{
        As<TrackBarThumb>(shared_from_this())
    });

    event_info.MarkAsHandled();
}


void TrackBarThumb::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() != MouseButton::Left) {
        return;
    }

    if (IsCapturingMouse()) {
        ReleaseMouse();
    }
    SetIsDragging(false);

    drag_ended_event_.Raise(TrackBarThumbDragEndedInfo{
        As<TrackBarThumb>(shared_from_this())
    });

    event_info.MarkAsHandled();
}


bool TrackBarThumb::IsDragging() const noexcept {
    return is_dragging_;
}


void TrackBarThumb::SetIsDragging(bool is_dragging) {

    is_dragging_ = is_dragging;
    NeedUpdateStyle();
    NeedRepaint();
}


bool TrackBarThumb::IsHorizontal() const noexcept {
    return is_horizontal_;
}

void TrackBarThumb::SetIsHorizontal(bool is_horizontal) {

    if (is_horizontal_ == is_horizontal) {
        return;
    }

    is_horizontal_ = is_horizontal;
    NeedRelayout();
    NeedRepaint();
}


Color TrackBarThumb::ThumbColor() const {
    return thumb_color_field_.Color();
}

void TrackBarThumb::SetThumbColor(const Color& color) {
    thumb_color_field_.SetColor(color, *this);
}


const ColorPicker& TrackBarThumb::ThumbColorPicker() const {
    return thumb_color_field_.ColorPicker();
}

void TrackBarThumb::SetThumbColorPicker(ColorPicker color_picker) {
    thumb_color_field_.SetColorPicker(std::move(color_picker), *this);
}


Observable<TrackBarThumbDragStartedInfo> TrackBarThumb::DragStartedEvent() const {
    return drag_started_event_.GetObservable();
}


Observable<TrackBarThumbDraggingInfo> TrackBarThumb::DraggingEvent() const {
    return dragging_event_.GetObservable();
}


Observable<TrackBarThumbDragEndedInfo> TrackBarThumb::DragEndedEvent() const {
    return drag_ended_event_.GetObservable();
}


TrackBarThumbDragStartedInfo::TrackBarThumbDragStartedInfo(
    std::shared_ptr<TrackBarThumb> source)
    :
    EventInfo(std::move(source)) {

}


TrackBarThumbDraggingInfo::TrackBarThumbDraggingInfo(std::shared_ptr<TrackBarThumb> source) : 
    EventInfo(std::move(source)) {

}


TrackBarThumbDragEndedInfo::TrackBarThumbDragEndedInfo(std::shared_ptr<TrackBarThumb> source) :
    EventInfo(std::move(source)) {

}

}