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
    ZAF_OBJECT;

public:
    ScrollBarArrow();

    /**
     Get arrow color.
     */
    Color ArrowColor() const;

    /**
     Set arrow color.
     */
    void SetArrowColor(const Color& color);

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
        return begin_press_event_.AsObservable();
    }

    /**
     Get end press event.

     This event is raised when the mouse left button is released after being pressed for a while.
     */
    Observable<ScrollBarArrowEndPressInfo> EndPressEvent() {
        return end_press_event_.AsObservable();
    }

protected:
    void Initialize() override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;
    void OnMouseCaptured(const MouseCapturedInfo& event_info) override;
    void OnMouseReleased(const MouseReleasedInfo& event_info) override;

private:
    friend class ScrollBar;

    void SetDirection(zaf::ArrowDirection direction) {
        arrow_direction_ = direction;
        NeedRepaint();
    }

private:
    zaf::ArrowDirection arrow_direction_;

    Color arrow_color_;

    Subject<ScrollBarArrowBeginPressInfo> begin_press_event_;
    Subject<ScrollBarArrowEndPressInfo> end_press_event_;
};

ZAF_OBJECT_BEGIN(ScrollBarArrow);
ZAF_OBJECT_PROPERTY(ArrowColor)
ZAF_OBJECT_PROPERTY(ArrowDirection)
ZAF_OBJECT_END;

}