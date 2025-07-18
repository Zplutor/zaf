#pragma once

#include <zaf/control/control.h>
#include <zaf/control/track_bar_thumb.h>

namespace zaf {

class TrackBarValueChangedInfo;

class TrackBar : public Control {
public:
    ZAF_OBJECT;

    const std::shared_ptr<TrackBarThumb>& Thumb() const noexcept;
    void SetThumb(std::shared_ptr<TrackBarThumb> thumb);

    int Value() const noexcept;
    void SetValue(int value);
    rx::Observable<TrackBarValueChangedInfo> ValueChangedEvent() const;

    int MinValue() const noexcept;
    void SetMinValue(int value);

    int MaxValue() const noexcept;
    void SetMaxValue(int value);

    int LargeChange() const noexcept;
    void SetLargeChange(int change) noexcept;

    bool IsHorizontal() const noexcept;
    void SetIsHorizontal(bool is_horizontal);

    float TrackThickness() const noexcept;
    void SetTrackThickness(float thickness);

    const Color& TrackColor() const noexcept;
    void SetTrackColor(const Color& color);

    const ColorPicker& TrackColorPicker() const noexcept;
    void SetTrackColorPicker(ColorPicker picker);

    float ThumbThickness() const noexcept;
    void SetThumbThickness(float thickness);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;

    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;

private:
    enum class Zone {
        Leading,
        Thumb,
        Tailing,
    };

private:
    void StartPressing();
    void ContinuePressing();
    Zone DetermineMouseZone() const;
    void ChangeValueByPressingZone();
    void StartPressingTimer(bool is_initial);

    void OnThumbDragStarted(const TrackBarThumbDragStartedInfo& event_info);
    void OnThumbDragging(const TrackBarThumbDraggingInfo& event_info);
    void OnThumbDragEnded(const TrackBarThumbDragEndedInfo& event_info);

private:
    std::shared_ptr<TrackBarThumb> thumb_;
    rx::SubscriptionBag thumb_subs_;

    int value_{};
    int min_value_{};
    int max_value_{ 10 };
    int large_change_{ 5 };

    bool is_horizontal_{ true };
    float track_thickness_{ 2.f };
    float thumb_thickness_{ 10.f };

    internal::ColorField track_color_field_;

    std::optional<Zone> pressing_zone_;
    rx::Subscription timer_sub_;

    float drag_start_position_{};
    int drag_start_value_{};

    Event<TrackBarValueChangedInfo> value_changed_event_;
};

ZAF_OBJECT_BEGIN(TrackBar);
ZAF_OBJECT_PROPERTY(IsHorizontal);
ZAF_OBJECT_PROPERTY(LargeChange);
ZAF_OBJECT_PROPERTY(MaxValue);
ZAF_OBJECT_PROPERTY(MinValue);
ZAF_OBJECT_PROPERTY(Thumb);
ZAF_OBJECT_PROPERTY(ThumbThickness);
ZAF_OBJECT_PROPERTY(TrackColor);
ZAF_OBJECT_PROPERTY(TrackColorPicker);
ZAF_OBJECT_PROPERTY(TrackThickness);
ZAF_OBJECT_PROPERTY(Value);
ZAF_OBJECT_END;


class TrackBarValueChangedInfo : public EventInfo {
public:
    explicit TrackBarValueChangedInfo(std::shared_ptr<TrackBar> track_bar);
};

}