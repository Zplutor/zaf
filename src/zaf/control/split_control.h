#pragma once

#include <optional>
#include <zaf/control/control.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_holder.h>

namespace zaf {

class SplitBar;
class SplitBarBeginDragInfo;
class SplitBarDragInfo;
class SplitBarEndDragInfo;
class SplitControlSplitDistanceChangeInfo;

class SplitControl : public Control {
public:
    ZAF_DECLARE_TYPE

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

    Observable<SplitControlSplitDistanceChangeInfo> SplitBarDistanceChangeEvent();

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
    void OnRectChanged(const zaf::Rect& previous_rect) override;

    virtual void OnSplitBarChanged(const std::shared_ptr<zaf::SplitBar>& previous_split_bar) { }
    virtual void OnFirstPaneChanged(const std::shared_ptr<Control>& previous_pane) { }
    virtual void OnSecondPaneChanged(const std::shared_ptr<Control>& previous_pane) { }

private:
    void InitializeSplitBar();
    void UninitializeSplitBar();

    bool UpdateActualSplitDistance();
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

    zaf::SubscriptionHolder split_bar_subscriptions_;

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
};


class SplitControlSplitDistanceChangeInfo {
public:
    SplitControlSplitDistanceChangeInfo(
        const std::shared_ptr<SplitControl>& split_control,
        float previous_distance)
        : 
        split_control_(split_control),
        previous_distance_(previous_distance) {

    }

    const std::shared_ptr<SplitControl>& SplitControl() const {
        return split_control_;
    }

    float PreviousDistance() const {
        return previous_distance_;
    }

private:
    std::shared_ptr<zaf::SplitControl> split_control_;
    float previous_distance_{};
};


class SplitBar : public Control {
public:
    ZAF_DECLARE_TYPE

public:
    SplitBar();

    bool IsHorizontal() const {
        return is_horizontal_;
    }

    void SetIsHorizontal(bool is_horizontal) {
        is_horizontal_ = is_horizontal;
    }

    ColorPicker SplitterColorPicker() const;
    void SetSplitterColorPicker(const ColorPicker& color_picker);

    Color SplitterColor() const {
        return SplitterColorPicker()(*this);
    }

    void SetSplitterColor(const Color& color) {
        SetSplitterColorPicker([color](const Control&) { return color; });
    }

    Observable<SplitBarBeginDragInfo> BeginDragEvent() {
        return begin_drag_event_.GetObservable();
    }

    Observable<SplitBarDragInfo> DragEvent() {
        return drag_event_.GetObservable();
    }

    Observable<SplitBarEndDragInfo> EndDragEvent() {
        return end_drag_event_.GetObservable();
    }

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    void ChangeMouseCursor(const Message& message, bool& is_changed) override;
    bool OnMouseMove(const Point& position, const MouseMessage& message) override;
    bool OnMouseDown(const Point& position, const MouseMessage& message) override;
    bool OnMouseUp(const Point& position, const MouseMessage& message) override;
    void OnMouseCapture() override;
    void OnMouseRelease() override;

private:
    bool is_horizontal_ = false;

    Subject<SplitBarBeginDragInfo> begin_drag_event_;
    Subject<SplitBarDragInfo> drag_event_;
    Subject<SplitBarEndDragInfo> end_drag_event_;
};


class SplitBarBeginDragInfo {
public:
    std::shared_ptr<SplitBar> split_bar;
};

class SplitBarDragInfo {
public:
    std::shared_ptr<SplitBar> split_bar;
};

class SplitBarEndDragInfo {
public:
    std::shared_ptr<SplitBar> split_bar;
};

}