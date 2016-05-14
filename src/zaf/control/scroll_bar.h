#pragma once

#include <zaf/control/clickable_control.h>
#include <zaf/base/event.h>

namespace zaf {

class Timer;

class ScrollBar : public Control {
public:
	class Arrow : public ClickableControl {
	public:
		enum class Direction {
			Left,
			Up,
			Right,
			Down,
		};

		typedef Event<const std::shared_ptr<Arrow>&> BeginPressEvent;
		typedef Event<const std::shared_ptr<Arrow>&> EndPressEvent;

	public:
		Arrow();

		void Initialize() override;

        const Color GetArrowColor() const {
            return GetArrowColorPicker()(*this);
        }
        const ColorPicker GetArrowColorPicker() const;
        void SetArrowColorPicker(const ColorPicker& color_picker);

		Direction GetDirection() const {
			return direction_;
		}

		void SetDirection(Direction direction) {
			direction_ = direction;
			NeedRepaint();
		}

		BeginPressEvent::Proxy GetBeginPressEvent() {
			return BeginPressEvent::Proxy(begin_press_event_);
		}

		EndPressEvent::Proxy GetEndPressEvent() {
			return EndPressEvent::Proxy(end_press_event_);
		}

	protected:
		void Paint(Canvas& canvas, const Rect& dirty_rect) override;
		void MouseCapture() override;
		void MouseRelease() override;

	private:
		Direction direction_;

		BeginPressEvent begin_press_event_;
		EndPressEvent end_press_event_;
	};

	class Thumb : public ClickableControl {
	public:
		typedef Event<const std::shared_ptr<Thumb>&> BeginDragEvent;
		typedef Event<const std::shared_ptr<Thumb>&> DragEvent;
		typedef Event<const std::shared_ptr<Thumb>&> EndDragEvent;

	public:
		Thumb();

		void Initialize() override;

		bool IsHorizontal() const {
			return is_horizontal_;
		}

		void SetIsHorizontal(bool is_horizontal) {
			is_horizontal_ = is_horizontal;
		}

		bool IsDragging() const {
			return is_dragging_;
		}

	public:
		BeginDragEvent::Proxy GetBeginDragEvent() {
			return BeginDragEvent::Proxy(begin_drag_event_);
		}

		DragEvent::Proxy GetDragEvent() {
			return DragEvent::Proxy(drag_event_);
		}

		EndDragEvent::Proxy GetEndDragEvent() {
			return EndDragEvent::Proxy(end_drag_event_);
		}

	protected:
		void MouseCapture() override;
		void MouseRelease() override;
		void MouseMove(const MouseMessage& message) override;

	private:
		bool is_horizontal_;
		bool is_dragging_;
		BeginDragEvent begin_drag_event_;
		DragEvent drag_event_;
		EndDragEvent end_drag_event_;
	};

	typedef Event<const std::shared_ptr<ScrollBar>&> ScrollEvent;

public:
	ScrollBar();

	void Initialize() override;

	/**
	 Set a new control for the incremental arrow.

	 Pass nullptr would reset the arrow to the default control.
	 */
	void SetIncrementalArrow(const std::shared_ptr<Arrow>& incremental_arrow);

	/**
	 Set a new control for the decremental arrow.

	 Pass nullptr would reset the arrow to the default control.
	 */
	void SetDecrementalArrow(const std::shared_ptr<Arrow>& decremental_arrow);

	/**
	 Set a new control for the thumb.

	 Pass nullptr would reset the thum to the default control.
	 */
	void SetThumb(const std::shared_ptr<Thumb>& thumb);

	bool IsHorizontal() const {
		return is_horizontal_;
	}

	void SetIsHorizontal(bool is_horizontal) {
		is_horizontal_ = is_horizontal;
		ApplyOrientationToChildren();
		NeedRelayout();
	}

	/**
	 Get the length of arrow buttons.

	 The default length is the same as scroll bar's thickness, 
     use SetArrowLength method to change it.
	 */
	float GetArrowLength() const;

	/**
	 Set the length of arrow length.
	 */
	void SetArrowLength(float length);

	int GetValue() const {
		return value_;
	}

	void SetValue(int value);

	int GetMinimumValue() const {
		return min_value_;
	}

	void SetMinimumValue(int value) {
		ChangeValueRange(value, max_value_, false);
	}

	int GetMaximumValue() const {
		return max_value_;
	}

	void SetMaximumValue(int value) {
		ChangeValueRange(min_value_, value, true);
	}

	void SetValueRange(int min_value, int max_value) {
		ChangeValueRange(min_value, max_value, false);
	}

    void Wheel(int distance);

	ScrollEvent::Proxy GetScrollEvent();

protected:
	void Layout(const Rect& previous_rect) override;

	void MouseDown(const MouseMessage& message) override;
    void MouseUp(const MouseMessage& message) override;
    void MouseWheel(const MouseWheelMessage& message) override;
	void MouseCapture() override;
	void MouseRelease() override;

private:
	enum class TimerEvent {
		Increment,
		Decrement,
		PageRoll,
	};

private:
	void InitializeArrow(const std::shared_ptr<Arrow>& arrow);
	void UninitializeArrow(const std::shared_ptr<Arrow>& arrow);
	void InitializeThumb(const std::shared_ptr<Thumb>& thumb);
	void UninitializeThumb(const std::shared_ptr<Thumb>& thumb);
	void ApplyOrientationToChildren();

	void CalculateThumbPositionAndLength(float slot_length, 
										 float& thumb_position, 
										 float& thumb_length);
	void ChangeVerticalRectToHorizontalRect(Rect& rect);

	void ChangeValueRange(int min_value, int max_value, bool max_value_has_priority);

	void BeginTimer(TimerEvent timer_event);
	void TimerTrigger(Timer&);
	void ApplyTimerEvent();
	Rect GetThumbSlotRect() const;

	void ArrowBeginPress(const std::shared_ptr<Arrow>& arrow);
	void ArrowEndPress(const std::shared_ptr<Arrow>& arrow);

	void ThumbBeginDrag(const std::shared_ptr<Thumb>& thumb);
	void ThumbDrag(const std::shared_ptr<Thumb>& thumb);
	void ThumbEndDrag(const std::shared_ptr<Thumb>& thumb);
	int GetValuesPerThumbSlotPoint();

private:
	std::shared_ptr<Arrow> incremental_arrow_;
	std::shared_ptr<Arrow> decremental_arrow_;
	std::shared_ptr<Thumb> thumb_;

	bool is_horizontal_;

	int min_value_;
	int max_value_;
	int value_;

	int begin_drag_value_;
	Point begin_drag_mouse_position_;

	std::unique_ptr<Timer> timer_;
	TimerEvent timer_event_;
};

}