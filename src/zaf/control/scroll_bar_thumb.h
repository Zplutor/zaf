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
    ZAF_OBJECT;

public:
    ScrollBarThumb();

    /**
     Get thumb color.
     */
    Color ThumbColor() const;

    /**
     Set thumb color.
     */
    void SetThumbColor(const Color& color);

    const ColorPicker& ThumbColorPicker() const;
    void SetThumbColorPicker(ColorPicker color_picker);

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
        return begin_drag_event_.AsObservable();
    }

    /**
     Get drag event.

     This event is raised when the thumb is beging dragged.
     */
    Observable<ScrollBarThumbDragInfo> DragEvent() {
        return drag_event_.AsObservable();
    }

    /**
     Get end drag event.

     This event is raised when the thumb has ended being dragged.
     */
    Observable<ScrollBarThumbEndDragInfo> EndDragEvent() {
        return end_drag_event_.AsObservable();
    }

protected:
    void Initialize() override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;
    void OnMouseCaptured(const MouseCapturedInfo& event_info) override;
    void OnMouseReleased(const MouseReleasedInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;

private:
    friend class ScrollBar;

    void SetIsHorizontal(bool is_horizontal);

private:
    bool is_horizontal_{};
    bool is_dragging_{};

    internal::ColorField thumb_color_field_;

    Subject<ScrollBarThumbBeginDragInfo> begin_drag_event_;
    Subject<ScrollBarThumbDragInfo> drag_event_;
    Subject<ScrollBarThumbEndDragInfo> end_drag_event_;
};

ZAF_OBJECT_BEGIN(ScrollBarThumb);
ZAF_OBJECT_PROPERTY(ThumbColor)
ZAF_OBJECT_PROPERTY(IsHorizontal)
ZAF_OBJECT_END;

}