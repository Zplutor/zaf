#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/rx/subject.h>

namespace zaf {

class ScrollBarArrowBeginPressInfo;
class ScrollBarArrowEndPressInfo;

/**
 Represents an arrow control in a scroll bar.
 */
class ScrollBarArrow : public ClickableControl {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
    /**
     Defines directions of an arrow.
     */
    enum class Direction {

        /**
         The arrow's direction is left.
         */
        Left,

        /**
         The arrow's direction is up.
         */
        Up,

        /**
         The arrow's direction is right.
        */
        Right,

        /**
         The arrow's direction is down.
        */
        Down,
    };

public:
    ScrollBarArrow();

    /**
     Get arrow color.
     */
    Color GetArrowColor() const {
        return GetArrowColorPicker()(*this);
    }

    /**
     Get the color picker of arrow.
     */
    ColorPicker GetArrowColorPicker() const;

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
    Direction GetDirection() const {
        return direction_;
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
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    void MouseCapture() override;
    void MouseRelease() override;

private:
    friend class ScrollBar;

    void SetDirection(Direction direction) {
        direction_ = direction;
        NeedRepaint();
    }

private:
    Direction direction_;

    Subject<ScrollBarArrowBeginPressInfo> begin_press_event_;
    Subject<ScrollBarArrowEndPressInfo> end_press_event_;
};


class ScrollBarArrowBeginPressInfo {
public:
    std::shared_ptr<ScrollBarArrow> scroll_bar_arrow;
};

class ScrollBarArrowEndPressInfo {
public:
    std::shared_ptr<ScrollBarArrow> scroll_bar_arrow;
};

}