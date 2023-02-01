#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/control/event/scroll_bar_thumb_event_info.h>
#include <zaf/rx/subject.h>

namespace zaf {

/**
 Represents a thumb control in a control.
 */
class ScrollBarThumb : public ClickableControl {
public:
    ZAF_DECLARE_TYPE

public:
    ScrollBarThumb();

    /**
     Get thumb color.
     */
    Color ThumbColor() const {
        return ThumbColorPicker()(*this);
    }

    /**
     Get the color picker of thumb.
     */
    ColorPicker ThumbColorPicker() const;

    /**
     Set thumb color.
     */
    void SetThumbColor(const Color& color) {
        SetThumbColorPicker(CreateColorPicker(color));
    }

    /**
     Set the color picker of thumb.
     */
    void SetThumbColorPicker(const ColorPicker& color_picker);

    /**
     Get a value indicating that whether the thumb is horizontal.

     The default value is false.
     */
    bool IsHorizontal() const {
        return is_horizontal_;
    }

    /**
     Get a value indicating that whether the thumb is being dragged.
     */
    bool IsDragging() const {
        return is_dragging_;
    }

    /**
     Get begin drag event.

     This event is raised when the thumb has begain being dragged.
     */
    Observable<ScrollBarThumbBeginDragInfo> BeginDragEvent() {
        return begin_drag_event_.GetObservable();
    }

    /**
     Get drag event.

     This event is raised when the thumb is beging dragged.
     */
    Observable<ScrollBarThumbDragInfo> DragEvent() {
        return drag_event_.GetObservable();
    }

    /**
     Get end drag event.

     This event is raised when the thumb has ended being dragged.
     */
    Observable<ScrollBarThumbEndDragInfo> EndDragEvent() {
        return end_drag_event_.GetObservable();
    }

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect);
    void OnMouseCapture() override;
    void OnMouseRelease() override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;

private:
    friend class ScrollBar;

    void SetIsHorizontal(bool is_horizontal) {
        is_horizontal_ = is_horizontal;
    }

private:
    bool is_horizontal_{};
    bool is_dragging_{};

    Subject<ScrollBarThumbBeginDragInfo> begin_drag_event_;
    Subject<ScrollBarThumbDragInfo> drag_event_;
    Subject<ScrollBarThumbEndDragInfo> end_drag_event_;
};

}