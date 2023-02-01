#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/control/arrow_direction.h>
#include <zaf/control/event/scroll_bar_arrow_event_info.h>
#include <zaf/rx/subject.h>

namespace zaf {

/**
 Represents an arrow control in a scroll bar.
 */
class ScrollBarArrow : public ClickableControl {
public:
    ZAF_DECLARE_TYPE

public:
    ScrollBarArrow();

    /**
     Get arrow color.
     */
    Color ArrowColor() const {
        return ArrowColorPicker()(*this);
    }

    /**
     Get the color picker of arrow.
     */
    ColorPicker ArrowColorPicker() const;

    /**
     Set arrow color.
     */
    void SetArrowColor(const Color& color) {
        SetArrowColorPicker(CreateColorPicker(color));
    }

    /**
     Set the color picker of arrow.
     */
    void SetArrowColorPicker(const ColorPicker& color_picker);

    /**
     Get the arrow's direction.

     The default direction is up.
     */
    zaf::ArrowDirection ArrowDirection() const {
        return arrow_direction_;
    }

    /**
     Get begin press event.

     This event is raised when the mouse left button is pressed for a while.
     */
    Observable<ScrollBarArrowBeginPressInfo> BeginPressEvent() {
        return begin_press_event_.GetObservable();
    }

    /**
     Get end press event.

     This event is raised when the mouse left button is released after being pressed for a while.
     */
    Observable<ScrollBarArrowEndPressInfo> EndPressEvent() {
        return end_press_event_.GetObservable();
    }

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    void OnMouseCapture() override;
    void OnMouseRelease() override;

private:
    friend class ScrollBar;

    void SetDirection(zaf::ArrowDirection direction) {
        arrow_direction_ = direction;
        NeedRepaint();
    }

private:
    zaf::ArrowDirection arrow_direction_;

    Subject<ScrollBarArrowBeginPressInfo> begin_press_event_;
    Subject<ScrollBarArrowEndPressInfo> end_press_event_;
};

}