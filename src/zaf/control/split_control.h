#pragma once

#include <optional>
#include <zaf/control/control.h>
#include <zaf/control/event/split_bar_event_info.h>
#include <zaf/control/event/split_distance_changed_info.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_set.h>

namespace zaf {

class SplitBar;

class SplitControl : public Control {
public:
    ZAF_OBJECT;

public:
    SplitControl();
    ~SplitControl();

    bool IsHorizontalSplit() const {
        return is_horizontal_split_;
    }

    void SetIsHorizontalSplit(bool is_horizontal);

    float SplitBarThickness() const {
        return split_bar_thickness_;
    }

    void SetSplitBarThickness(float thickness);

    float SplitDistance() const {
        return actual_split_distance_;
    }

    void SetSplitDistance(float distance);

    Observable<SplitDistanceChangedInfo> SplitDistanceChangedEvent() const;

    const std::shared_ptr<zaf::SplitBar>& SplitBar() const {
        return split_bar_;
    }

    void SetSplitBar(const std::shared_ptr<zaf::SplitBar>& split_bar);

    const std::shared_ptr<Control>& FirstPane() const {
        return first_pane_;
    }

    void SetFirstPane(const std::shared_ptr<Control>& pane);

    const std::shared_ptr<Control>& SecondPane() const {
        return second_pane_;
    }

    void SetSecondPane(const std::shared_ptr<Control>& pane);

    float FirstPaneMinLength() const {
        return first_pane_min_length_;
    }

    void SetFirstPaneMinLength(float length);

    float FirstPaneMaxLength() const {
        return first_pane_max_length_;
    }

    void SetFirstPaneMaxLength(float length);

    float SecondPaneMinLength() const {
        return second_pane_min_length_;
    }

    void SetSecondPaneMinLength(float length);

    float SecondPaneMaxLength() const {
        return second_pane_max_length_;
    }

    void SetSecondPaneMaxLength(float length);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;
    void OnRectChanged(const RectChangedInfo& event_info) override;

    virtual void OnSplitBarChanged(const std::shared_ptr<zaf::SplitBar>& previous_split_bar) { }
    virtual void OnFirstPaneChanged(const std::shared_ptr<Control>& previous_pane) { }
    virtual void OnSecondPaneChanged(const std::shared_ptr<Control>& previous_pane) { }

private:
    void InitializeSplitBar();
    void UninitializeSplitBar();

    void InnerSetSplitDistance(float distance, bool is_by_dragging);

    bool UpdateActualSplitDistance(bool is_by_dragging);
    void GetSplitDistanceLimit(float& total_length, float& min, float& max) const;

    void SetPane(
        std::shared_ptr<Control>& pane,
        const std::shared_ptr<Control>& new_pane, 
        const std::function<void(const std::shared_ptr<Control>&)>& notification);

    void SetPaneLimitLength(float new_length, bool is_setting_min, float& min, float& max);

    void SplitBarBeginDrag();
    void SplitBarDrag();
    void SplitBarEndDrag();
    float GetSplitBarDragPosition() const;

private:
    std::shared_ptr<zaf::SplitBar> split_bar_;
    std::shared_ptr<Control> first_pane_;
    std::shared_ptr<Control> second_pane_;

    zaf::SubscriptionSet split_bar_subscriptions_;

    bool is_horizontal_split_{};
    float split_bar_thickness_{ 3.f };
    float first_pane_min_length_{};
    float first_pane_max_length_{ (std::numeric_limits<float>::max)() };
    float second_pane_min_length_{};
    float second_pane_max_length_{ (std::numeric_limits<float>::max)() };
    float actual_split_distance_ = 0;
    std::optional<float> expected_split_distance_;

    float split_bar_begin_drag_mouse_position_ = 0;
    float split_bar_begin_drag_distance_ = 0;

    Event<SplitDistanceChangedInfo> split_distance_changed_event_;
};


class SplitBar : public Control {
public:
    ZAF_OBJECT;

public:
    SplitBar();

    bool IsHorizontal() const {
        return is_horizontal_;
    }

    void SetIsHorizontal(bool is_horizontal) {
        is_horizontal_ = is_horizontal;
    }

    Color SplitterColor() const;
    void SetSplitterColor(const Color& color);

    const ColorPicker& SplitterColorPicker() const;
    void SetSplitterColorPicker(ColorPicker picker);

    Observable<SplitBarBeginDragInfo> BeginDragEvent() {
        return begin_drag_event_.AsObservable();
    }

    Observable<SplitBarDragInfo> DragEvent() {
        return drag_event_.AsObservable();
    }

    Observable<SplitBarEndDragInfo> EndDragEvent() {
        return end_drag_event_.AsObservable();
    }

protected:
    void Initialize() override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;
    void OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnMouseCaptured(const MouseCapturedInfo& event_info) override;
    void OnMouseReleased(const MouseReleasedInfo& event_info) override;

private:
    bool is_horizontal_ = false;

    internal::ColorField splitter_color_field_;

    Subject<SplitBarBeginDragInfo> begin_drag_event_;
    Subject<SplitBarDragInfo> drag_event_;
    Subject<SplitBarEndDragInfo> end_drag_event_;
};


ZAF_OBJECT_BEGIN(SplitControl);
ZAF_OBJECT_PROPERTY(IsHorizontalSplit)
ZAF_OBJECT_PROPERTY(SplitBarThickness)
ZAF_OBJECT_PROPERTY(SplitDistance)
ZAF_OBJECT_PROPERTY(FirstPaneMinLength)
ZAF_OBJECT_PROPERTY(FirstPaneMaxLength)
ZAF_OBJECT_PROPERTY(SecondPaneMinLength)
ZAF_OBJECT_PROPERTY(SecondPaneMaxLength)
ZAF_OBJECT_PROPERTY(SplitBar)
ZAF_OBJECT_PROPERTY(FirstPane)
ZAF_OBJECT_PROPERTY(SecondPane)
ZAF_OBJECT_END;


ZAF_OBJECT_BEGIN(SplitBar);
ZAF_OBJECT_PROPERTY(IsHorizontal)
ZAF_OBJECT_PROPERTY(SplitterColor)
ZAF_OBJECT_PROPERTY(SplitterColorPicker)
ZAF_OBJECT_END;

}