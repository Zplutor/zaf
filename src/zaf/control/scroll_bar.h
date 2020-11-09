#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_holder.h>

namespace zaf {

class ScrollBarArrow;
class ScrollBarArrowBeginPressInfo;
class ScrollBarArrowEndPressInfo;
class ScrollBarScrollInfo;
class ScrollBarThumb;
class ScrollBarThumbBeginDragInfo;
class ScrollBarThumbDragInfo;
class ScrollBarThumbEndDragInfo;
class Timer;

/**
 Represents a scroll bar control.
 */
class ScrollBar : public Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
	ScrollBar();

    /**
     Get the incremental arrow.
     */
    const std::shared_ptr<ScrollBarArrow>& GetIncrementalArrow() const {
        return incremental_arrow_;
    }

	/**
	 Set a new control for the incremental arrow.

	 Pass nullptr would reset the arrow to the default control.
	 */
	void SetIncrementalArrow(const std::shared_ptr<ScrollBarArrow>& incremental_arrow);

    /**
     Get the decremental arrow.
     */
    const std::shared_ptr<ScrollBarArrow>& GetDecrementalArrow() const {
        return decremental_arrow_;
    }

	/**
	 Set a new control for the decremental arrow.

	 Pass nullptr would reset the arrow to the default control.
	 */
	void SetDecrementalArrow(const std::shared_ptr<ScrollBarArrow>& decremental_arrow);

    /**
     Get the thumb.
     */
    const std::shared_ptr<ScrollBarThumb>& GetThumb() const {
        return thumb_;
    }

	/**
	 Set a new control for the thumb.

	 Pass nullptr would reset the thum to the default control.
	 */
	void SetThumb(const std::shared_ptr<ScrollBarThumb>& thumb);

    /**
     Get a value indicating that whether the scroll bar is horizontal.

     The default value is false.
     */
	bool IsHorizontal() const {
		return is_horizontal_;
	}

    /**
     Set a value indicating that whether the scroll bar is horizontal.
     */
	void SetIsHorizontal(bool is_horizontal) {
		is_horizontal_ = is_horizontal;
		ApplyOrientationToChildren();
		NeedRelayout();
	}

	/**
	 Get the length of arrow controls.

	 The default length is the same as scroll bar's thickness, 
     use SetArrowLength method to change it.
	 */
	float GetArrowLength() const;

	/**
	 Set the length of arrow controls.
	 */
	void SetArrowLength(float length);

    /**
     Get the scroll value.

     The default value is 0.
     */
	int GetValue() const {
		return value_;
	}

    /**
     Set the scroll value.

     The value would be adjusted to the minimum or maximum if it exceeds the 
     allowed range.

     Settting the value would change the thumb's position immediately.
     */
	void SetValue(int value);

    /**
     Get minium scroll value.

     The default value is 0.
     */
	int GetMinimumValue() const {
		return min_value_;
	}

    /**
     Set minimum scroll value.

     The scroll value would be adjusted to the minimum if the current value
     is less than it.
     */
	void SetMinimumValue(int value) {
		ChangeValueRange(value, max_value_, false);
	}

    /**
     Get maximum scroll value.

     The default value is 0.
     */
	int GetMaximumValue() const {
		return max_value_;
	}

    /**
     Set maximum scroll value.

     The scroll value would be adjusted to the maximum if the current value
     is greater than it.
     */
	void SetMaximumValue(int value) {
		ChangeValueRange(min_value_, value, true);
	}

    /**
     Set allowed scroll value range.

     @param min_value
        The minimum scroll value.

     @param max_value
        The maximum scroll value.

     This is a shortcut that can be used to set both minimum scroll value
     and maximum scroll value.

     Maximum value would be adjusted to the minimum if the maximum is less than
     the minimum. The scroll value would be adjusted if the current value exceeds the 
     allowed range.
     */
	void SetValueRange(int min_value, int max_value) {
		ChangeValueRange(min_value, max_value, false);
	}

    int GetSmallChangeValue() const;
    void SetSmallChangeValue(int value);

    int GetLargeChangeValue() const;
    void SetLargeChangeValue(int value);

    /**
     Perform a wheel action on the scroll bar.

     @param distance
        The wheeling distance.
     */
    void Wheel(int distance);

    /**
     Get scroll event.

     This event is raised when the scroll value is changed.
     */
	Observable<ScrollBarScrollInfo> ScrollEvent();

protected:
    void Initialize() override;
	void Layout(const Rect& previous_rect) override;

    bool MouseDown(const Point& position, const MouseMessage& message) override;
    bool MouseUp(const Point& position, const MouseMessage& message) override;
    bool MouseWheel(const Point& position, const MouseWheelMessage& message) override;
	void MouseCapture() override;
	void MouseRelease() override;

private:
	enum class TimerEvent {
		Increment,
		Decrement,
		PageRoll,
	};

private:
	void InitializeArrow(const std::shared_ptr<ScrollBarArrow>& arrow, bool is_incremental);
	void UninitializeArrow(const std::shared_ptr<ScrollBarArrow>& arrow, bool is_incremental);
	void InitializeThumb(const std::shared_ptr<ScrollBarThumb>& thumb);
	void UninitializeThumb(const std::shared_ptr<ScrollBarThumb>& thumb);
	void ApplyOrientationToChildren();

	void CalculateThumbPositionAndLength(float slot_length, 
										 float& thumb_position, 
										 float& thumb_length);
	void ChangeVerticalRectToHorizontalRect(Rect& rect);

	void ChangeValueRange(int min_value, int max_value, bool max_value_has_priority);

	void BeginTimer(TimerEvent timer_event);
	void TimerTrigger();
	void ApplyTimerEvent();
	Rect GetThumbSlotRect() const;

	void ArrowBeginPress(const ScrollBarArrowBeginPressInfo& event_info);
	void ArrowEndPress(const ScrollBarArrowEndPressInfo& event_info);

	void ThumbBeginDrag(const ScrollBarThumbBeginDragInfo& event_info);
	void ThumbDrag(const ScrollBarThumbDragInfo& event_info);
	void ThumbEndDrag(const ScrollBarThumbEndDragInfo& event_info);
	int GetValuesPerThumbSlotPoint();

private:
	std::shared_ptr<ScrollBarArrow> incremental_arrow_;
	std::shared_ptr<ScrollBarArrow> decremental_arrow_;
	std::shared_ptr<ScrollBarThumb> thumb_;

    zaf::SubscriptionHolder incremental_arrow_subscriptions_;
    zaf::SubscriptionHolder decremental_arrow_subscriptions_;
    zaf::SubscriptionHolder thumb_subscriptions_;

	bool is_horizontal_;

	int min_value_;
	int max_value_;
	int value_;

	int begin_drag_value_;
	Point begin_drag_mouse_position_;

	std::shared_ptr<Timer> timer_;
	TimerEvent timer_event_;

    double remaining_wheel_change_value_;
};


class ScrollBarScrollInfo {
public:
    std::shared_ptr<ScrollBar> scroll_bar;
};


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


/**
 Represents a thumb control in a control.
 */
class ScrollBarThumb : public ClickableControl {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
	ScrollBarThumb();

    /**
     Get thumb color.
     */
    Color GetThumbColor() const {
        return GetThumbColorPicker()(*this);
    }

    /**
     Get the color picker of thumb.
     */
    ColorPicker GetThumbColorPicker() const;

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
    void Paint(Canvas& canvas, const Rect& dirty_rect);
	void MouseCapture() override;
	void MouseRelease() override;
    bool MouseMove(const Point& position, const MouseMessage& message) override;

private:
    friend class ScrollBar;

    void SetIsHorizontal(bool is_horizontal) {
        is_horizontal_ = is_horizontal;
    }

private:
	bool is_horizontal_;
	bool is_dragging_;

    Subject<ScrollBarThumbBeginDragInfo> begin_drag_event_;
	Subject<ScrollBarThumbDragInfo> drag_event_;
	Subject<ScrollBarThumbEndDragInfo> end_drag_event_;
};


class ScrollBarThumbBeginDragInfo {
public:
    std::shared_ptr<ScrollBarThumb> scroll_bar_thumb;
};

class ScrollBarThumbDragInfo {
public:
    std::shared_ptr<ScrollBarThumb> scroll_bar_thumb;
};

class ScrollBarThumbEndDragInfo {
public:
    std::shared_ptr<ScrollBarThumb> scroll_bar_thumb;
};

}