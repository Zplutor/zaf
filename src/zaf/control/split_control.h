#pragma once

#include <zaf/base/event.h>
#include <zaf/control/control.h>

namespace zaf {

class SplitControl : public Control {
public:
    class SplitBar : public Control {
    public:
        typedef Event<const std::shared_ptr<SplitBar>&> BeginDragEvent;
        typedef Event<const std::shared_ptr<SplitBar>&> DragEvent;
        typedef Event<const std::shared_ptr<SplitBar>&> EndDragEvent;

    public:
        void Initialize() override;

    public:
        bool IsHorizontal() const {
            return is_horizontal_;
        }

        void SetIsHorizontal(bool is_horizontal) {
            is_horizontal_ = is_horizontal;
        }

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
        void Paint(Canvas& canvas, const Rect& dirty_rect) override;
        void ChangeMouseCursor(const Message& message, bool& is_changed) override;
        void MouseMove(const Point& position, const MouseMessage& message) override;
        void MouseDown(const Point& position, const MouseMessage& message) override;
        void MouseUp(const Point& position, const MouseMessage& message) override;
        void MouseCapture() override;
        void MouseRelease() override;

    private:
        bool is_horizontal_ = false;
        BeginDragEvent begin_drag_event_;
        DragEvent drag_event_;
        EndDragEvent end_drag_event_;
    };

public:
    SplitControl();
    ~SplitControl();

    void Initialize() override;

    bool IsHorizontal() const;
    void SetIsHorizontal(bool is_horizontal);

    float GetSplitBarThickness() const;
    void SetSplitBarThickness(float thickness);

    float GetSplitBarDistance() const;
    void SetSplitBarDistance(float distance);

    float GetMinimumSplitBarDistance() const;
    void SetMinimumSplitBarDistance(float distance);

    float GetMaximumSplitBarDistance() const;
    void SetMaximumSplitBarDistance(float distance);

    const std::shared_ptr<SplitBar>& GetSplitBar() const {
        return split_bar_;
    }

    void SetSplitBar(const std::shared_ptr<SplitBar>& split_bar);

    const std::shared_ptr<Control>& GetFirstPane() const {
        return first_pane_;
    }

    void SetFirstPane(const std::shared_ptr<Control>& pane);

    const std::shared_ptr<Control>& GetSecondPane() const {
        return second_pane_;
    }

    void SetSecondPane(const std::shared_ptr<Control>& pane);

protected:
    void Layout(const Rect& previous_rect) override;

    virtual void SplitBarChange(const std::shared_ptr<SplitBar>& previous_split_bar) { }
    virtual void FirstPaneChange(const std::shared_ptr<Control>& previous_pane) { }
    virtual void SecondPaneChange(const std::shared_ptr<Control>& previous_pane) { }

private:
    void InitializeSplitBar();
    void UninitializeSplitBar();

    void SetPane(
        std::shared_ptr<Control>& pane,
        const std::shared_ptr<Control>& new_pane, 
        const std::function<void(const std::shared_ptr<Control>&)>& notification);

    void SplitBarBeginDrag();
    void SplitBarDrag();
    void SplitBarEndDrag();
    float GetSplitBarDragPosition() const;

private:
    std::shared_ptr<SplitBar> split_bar_;
    std::shared_ptr<Control> first_pane_;
    std::shared_ptr<Control> second_pane_;

    float split_bar_begin_drag_mouse_position_ = 0;
    float split_bar_begin_drag_distance_ = 0;
};

}