#pragma once

#include <optional>
#include <zaf/control/control.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_holder.h>

namespace zaf {

class SplitControlSplitBar;
class SplitControlSplitBarBeginDragInfo;
class SplitControlSplitBarDistanceChangeInfo;
class SplitControlSplitBarDragInfo;
class SplitControlSplitBarEndDragInfo;

class SplitControl : public Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
    SplitControl();
    ~SplitControl();

    bool IsHorizontalSplit() const;
    void SetIsHorizontalSplit(bool is_horizontal);

    float GetSplitBarThickness() const;
    void SetSplitBarThickness(float thickness);

    float GetSplitBarDistance() const;
    void SetSplitBarDistance(float distance);

    float GetMinSplitBarDistance() const;
    void SetMinSplitBarDistance(float distance);

    float GetMaxSplitBarDistance() const;
    void SetMaxSplitBarDistance(float distance);

    bool IsSplitBarDistanceFlipped() const;
    void SetIsSplitBarDistanceFlipped(bool is_flipped);

    Observable<SplitControlSplitBarDistanceChangeInfo> SplitBarDistanceChangeEvent();

    const std::shared_ptr<SplitControlSplitBar>& GetSplitBar() const {
        return split_bar_;
    }

    void SetSplitBar(const std::shared_ptr<SplitControlSplitBar>& split_bar);

    const std::shared_ptr<Control>& GetFirstPane() const {
        return first_pane_;
    }

    void SetFirstPane(const std::shared_ptr<Control>& pane);

    const std::shared_ptr<Control>& GetSecondPane() const {
        return second_pane_;
    }

    void SetSecondPane(const std::shared_ptr<Control>& pane);

protected:
    void Initialize() override;
    void Layout(const Rect& previous_rect) override;
    void OnRectChanged(const Rect& previous_rect) override;

    virtual void SplitBarChange(const std::shared_ptr<SplitControlSplitBar>& previous_split_bar) { }
    virtual void FirstPaneChange(const std::shared_ptr<Control>& previous_pane) { }
    virtual void SecondPaneChange(const std::shared_ptr<Control>& previous_pane) { }

private:
    void InitializeSplitBar();
    void UninitializeSplitBar();

    void UpdateActualSplitBarDistance();

    float GetAvaliableSplitBarMaxDistance() const;
    float GetUnflippedSplitBarDistance() const;

    void SetPane(
        std::shared_ptr<Control>& pane,
        const std::shared_ptr<Control>& new_pane, 
        const std::function<void(const std::shared_ptr<Control>&)>& notification);

    void SplitBarBeginDrag();
    void SplitBarDrag();
    void SplitBarEndDrag();
    float GetSplitBarDragPosition() const;

private:
    std::shared_ptr<SplitControlSplitBar> split_bar_;
    std::shared_ptr<Control> first_pane_;
    std::shared_ptr<Control> second_pane_;

    zaf::SubscriptionHolder split_bar_subscriptions_;

    float actual_split_bar_distance_ = 0;
    std::optional<float> expected_split_bar_distance_;
    std::optional<float> expected_split_bar_min_distance_;
    std::optional<float> expected_split_bar_max_distance_;

    float split_bar_begin_drag_mouse_position_ = 0;
    float split_bar_begin_drag_distance_ = 0;
};


class SplitControlSplitBarDistanceChangeInfo {
public:
    std::shared_ptr<SplitControl> split_control;
    float previous_distance{};
};


class SplitControlSplitBar : public Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
    SplitControlSplitBar();

    bool IsHorizontal() const {
        return is_horizontal_;
    }

    void SetIsHorizontal(bool is_horizontal) {
        is_horizontal_ = is_horizontal;
    }

    ColorPicker GetSplitterColorPicker() const;
    void SetSplitterColorPicker(const ColorPicker& color_picker);

    Color GetSplitterColor() const {
        return GetSplitterColorPicker()(*this);
    }

    void SetSplitterColor(const Color& color) {
        SetSplitterColorPicker([color](const Control&) { return color; });
    }

    Observable<SplitControlSplitBarBeginDragInfo> BeginDragEvent() {
        return begin_drag_event_.GetObservable();
    }

    Observable<SplitControlSplitBarDragInfo> DragEvent() {
        return drag_event_.GetObservable();
    }

    Observable<SplitControlSplitBarEndDragInfo> EndDragEvent() {
        return end_drag_event_.GetObservable();
    }

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    void ChangeMouseCursor(const Message& message, bool& is_changed) override;
    bool OnMouseMove(const Point& position, const MouseMessage& message) override;
    bool OnMouseDown(const Point& position, const MouseMessage& message) override;
    bool OnMouseUp(const Point& position, const MouseMessage& message) override;
    void OnMouseCapture() override;
    void OnMouseRelease() override;

private:
    bool is_horizontal_ = false;

    Subject<SplitControlSplitBarBeginDragInfo> begin_drag_event_;
    Subject<SplitControlSplitBarDragInfo> drag_event_;
    Subject<SplitControlSplitBarEndDragInfo> end_drag_event_;
};


class SplitControlSplitBarBeginDragInfo {
public:
    std::shared_ptr<SplitControlSplitBar> split_bar;
};

class SplitControlSplitBarDragInfo {
public:
    std::shared_ptr<SplitControlSplitBar> split_bar;
};

class SplitControlSplitBarEndDragInfo {
public:
    std::shared_ptr<SplitControlSplitBar> split_bar;
};

}