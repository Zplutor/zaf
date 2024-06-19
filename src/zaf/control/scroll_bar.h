#pragma once

#include <optional>
#include <zaf/control/clickable_control.h>
#include <zaf/control/event/scroll_bar_arrow_event_info.h>
#include <zaf/control/event/scroll_bar_scroll_info.h>
#include <zaf/control/event/scroll_bar_thumb_event_info.h>
#include <zaf/control/scroll_bar_arrow.h>
#include <zaf/control/scroll_bar_thumb.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_set.h>

namespace zaf {

class Timer;

/**
 Represents a scroll bar control.
 */
class ScrollBar : public Control {
public:
    ZAF_DECLARE_TYPE;
    ZAF_DECLARE_PROPERTY;

public:
    ScrollBar();

    /**
     Get the incremental arrow.
     */
    const std::shared_ptr<ScrollBarArrow>& IncrementalArrow() const {
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
    const std::shared_ptr<ScrollBarArrow>& DecrementalArrow() const {
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
    const std::shared_ptr<ScrollBarThumb>& Thumb() const {
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
    float ArrowLength() const;

    /**
     Set the length of arrow controls.
     */
    void SetArrowLength(float length);

    /**
     Get the scroll value.

     The default value is 0.
     */
    int Value() const {
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
    int MinValue() const {
        return min_value_;
    }

    /**
     Set minimum scroll value.

     The scroll value would be adjusted to the minimum if the current value
     is less than it.
     */
    void SetMinValue(int value) {
        ChangeValueRange(value, max_value_, false);
    }

    /**
     Get maximum scroll value.

     The default value is 0.
     */
    int MaxValue() const {
        return max_value_;
    }

    /**
     Set maximum scroll value.

     The scroll value would be adjusted to the maximum if the current value
     is greater than it.
     */
    void SetMaxValue(int value) {
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

    int SmallChange() const;
    void SetSmallChange(int value);

    int LargeChange() const;
    void SetLargeChange(int value);

    int PageSize() const {
        return page_size_;
    }
    void SetPageSize(int value);

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
    Observable<ScrollBarScrollInfo> ScrollEvent() const;

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;

    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnMouseWheel(const MouseWheelInfo& event_info) override;
    void OnMouseCaptured(const MouseCapturedInfo& event_info) override;
    void OnMouseReleased(const MouseReleasedInfo& event_info) override;

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

    void CalculateThumbPositionAndLength(
        float track_length,
        float& thumb_position, 
        float& thumb_length);
    float CalculateThumbLength(float track_length);
    float CalculateThumbPosition(float track_length, float thumb_length);
    void ChangeVerticalRectToHorizontalRect(zaf::Rect& rect);

    void ChangeValueRange(int min_value, int max_value, bool max_value_has_priority);

    void BeginTimer(TimerEvent timer_event);
    void TimerTrigger();
    void ApplyTimerEvent();
    zaf::Rect GetThumbSlotRect() const;

    void ArrowBeginPress(const ScrollBarArrowBeginPressInfo& event_info);
    void ArrowEndPress(const ScrollBarArrowEndPressInfo& event_info);

    void ThumbBeginDrag(const ScrollBarThumbBeginDragInfo& event_info);
    void ThumbDrag(const ScrollBarThumbDragInfo& event_info);
    void ThumbEndDrag(const ScrollBarThumbEndDragInfo& event_info);
    float GetValuePerThumbSlotPoint();

private:
    std::shared_ptr<ScrollBarArrow> incremental_arrow_;
    std::shared_ptr<ScrollBarArrow> decremental_arrow_;
    std::shared_ptr<ScrollBarThumb> thumb_;

    zaf::SubscriptionSet incremental_arrow_subscriptions_;
    zaf::SubscriptionSet decremental_arrow_subscriptions_;
    zaf::SubscriptionSet thumb_subscriptions_;

    std::optional<float> arrow_length_;

    bool is_horizontal_;

    int min_value_;
    int max_value_;
    int value_;

    int page_size_{};
    int small_change_{ 1 };
    std::optional<int> large_change_;

    int begin_drag_value_;
    Point begin_drag_mouse_position_;

    bool is_pressing_{};
    std::shared_ptr<Timer> timer_;
    TimerEvent timer_event_;

    double remaining_wheel_change_value_;

    Event<ScrollBarScrollInfo> scroll_event_;
};

ZAF_PROPERTY_BEGIN(ScrollBar)
ZAF_PROPERTY(IncrementalArrow)
ZAF_PROPERTY(DecrementalArrow)
ZAF_PROPERTY(Thumb)
ZAF_PROPERTY(IsHorizontal)
ZAF_PROPERTY(ArrowLength)
ZAF_PROPERTY(Value)
ZAF_PROPERTY(MinValue)
ZAF_PROPERTY(MaxValue)
ZAF_PROPERTY(SmallChange)
ZAF_PROPERTY(LargeChange)
ZAF_PROPERTY(PageSize)
ZAF_PROPERTY_END

}