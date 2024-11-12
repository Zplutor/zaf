#pragma once

#include <zaf/control/control.h>

namespace zaf {

class TrackBarThumbDragStartedInfo;
class TrackBarThumbDraggingInfo;
class TrackBarThumbDragEndedInfo;

class TrackBarThumb : public Control {
public:
    bool IsHorizontal() const noexcept;
    void SetIsHorizontal(bool is_horizontal);

    bool IsDragging() const noexcept;

    Color ThumbColor() const;
    void SetThumbColor(const Color& color);

    const ColorPicker& ThumbColorPicker() const;
    void SetThumbColorPicker(ColorPicker color_picker);

    Observable<TrackBarThumbDragStartedInfo> DragStartedEvent() const;
    Observable<TrackBarThumbDraggingInfo> DraggingEvent() const;
    Observable<TrackBarThumbDragEndedInfo> DragEndedEvent() const;

protected:
    void Initialize() override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;

    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;

private:
    void SetIsDragging(bool is_dragging);

private:
    bool is_horizontal_{ true };
    bool is_dragging_{ false };

    internal::ColorField thumb_color_field_;

    Event<TrackBarThumbDragStartedInfo> drag_started_event_;
    Event<TrackBarThumbDraggingInfo> dragging_event_;
    Event<TrackBarThumbDragEndedInfo> drag_ended_event_;
};


class TrackBarThumbDragStartedInfo : public EventInfo {
public:
    explicit TrackBarThumbDragStartedInfo(std::shared_ptr<TrackBarThumb> source);
};


class TrackBarThumbDraggingInfo : public EventInfo {
public:
    explicit TrackBarThumbDraggingInfo(std::shared_ptr<TrackBarThumb> source);
};


class TrackBarThumbDragEndedInfo : public EventInfo {
public:
    explicit TrackBarThumbDragEndedInfo(std::shared_ptr<TrackBarThumb> source);
};

}