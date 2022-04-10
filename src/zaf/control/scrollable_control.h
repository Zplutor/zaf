#pragma once

#include <zaf/control/control.h>
#include <zaf/rx/subscription.h>

namespace zaf {
namespace internal {
class GeneralScrollingLayouter;
class ScrollableControlLayouter;
class SelfScrollingLayouter;
}

class ScrollBar;
class SelfScrollingControl;
class zaf::Size;

/**
 Represents a scrollable control.

 This is the base class of all scrollable control. 
 
 The scrollable control contains a scroll content control, which would be scrolled when the scroll
 bars changes. You can obatin this control by calling ScrollContent method, and then add
 children to it to build the scroll page. You can also set a custom scroll content control by 
 calling SetScrollContent method.

 By default, the size of scroll content control would be auto set to scrollable control's visible 
 area size, thus scroll bars would not enabled. To enable scroll bars, you can call SetFixedWidth or
 SetFixedHeight to scroll content control to prevent auto-sizing. Or you can call 
 SetAutoScrollContentWidth and SetAutoScrollContentHeight to disable auto-sizing.

 The scroll content control can inherits SelfScrollingControl class. In this way, its scrollable control
 doesn't scroll it anymore, instead, the scroll content control scrolls its own content and controls 
 the scroll bars. For more information, see also SelfScrollingControl.
 */
class ScrollableControl : public Control {
public:
    ZAF_DECLARE_TYPE

public:
    ScrollableControl();
    ~ScrollableControl();

    /**
     Get a value indicating that whether the control allows vertical scrolling.

     The default value is true.
     */
    bool AllowVerticalScroll() const;

    /**
     Set a value indicating that whether the control allows vertical scrolling.
     */
    void SetAllowVerticalScroll(bool allow_scroll);

    /**
     Get a value indicating that whether the control allows horizontal scrolling.

     The default value is true.
     */
    bool AllowHorizontalScroll() const;

    /**
     Set a value indicating that whether the control allows horizontal scrolling.
     */
    void SetAllowHorizontalScroll(bool allow_scroll);

    /**
     Get a value indicating that whether the scroll bars auto hide when they are no needed.

     The default value is false.
     */
    bool AutoHideScrollBars() const;

    /**
     Set a value indicating that whether the scroll bars auto hide when they are no needed.
     */
    void SetAutoHideScrollBars(bool auto_hide);

    bool AutoScrollBarLargeChange() const;
    void SetAutoScrollBarLargeChange(bool value);

    /**
     Get the vertical scroll bar.
     */
    const std::shared_ptr<ScrollBar>& VerticalScrollBar() const {
        return vertical_scroll_bar_;
    }

    /**
     Set the vertical scroll bar.

     Pass nullptr would set back to the default vertical scroll bar.
     */
    void SetVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);

    /**
     Get the horizontal scroll bar.
     */
    const std::shared_ptr<ScrollBar>& HorizontalScrollBar() const {
        return horizontal_scroll_bar_;
    }

    /**
     Set the horizontal scroll bar.

     Pass nullptr would set back to the default horizontal scroll bar.
     */
    void SetHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);

    /**
     Get the scroll bar corner control.
     */
    const std::shared_ptr<Control>& ScrollBarCorner() const {
        return scroll_bar_corner_;
    }

    /**
     Set the scroll bar corner control.
     */
    void SetScrollBarCorner(const std::shared_ptr<Control>& control);

    /**
     Get the scroll content control.
     */
    const std::shared_ptr<Control>& ScrollContent() const {
        return scroll_content_control_;
    }

    /**
     Set the scroll content control.

     Pass nullptr would set back to the default scroll content control.
     */
    void SetScrollContent(const std::shared_ptr<Control>& control);

    bool AutoScrollContentWidth() const;
    void SetAutoScrollContentWidth(bool value);

    bool AutoScrollContentHeight() const;
    void SetAutoScrollContentHeight(bool value);

    void SetAutoScrollContentSize(bool value);

    /**
     Get scroll bar thickness.

     The default value is 17.
     */
    float ScrollBarThickness() const;

    /**
     Set scroll bar thickness.
     */
    void SetScrollBarThickness(float thickness);

    /**
     Get the visible scroll content rect, in scroll content control's coordinate.
     */
    zaf::Rect GetVisibleScrollContentRect() const;

    /**
     Scroll the scroll content to specified position, in scroll content control's coordinate.

     The specified position occurs at left-top cornor of the control after scrolling.
     */
    void ScrollToScrollContentPosition(const Point& position);

    void ScrollUpToBegin();
    void ScrollDownToEnd();
    void ScrollLeftToBegin();
    void ScrollRightToEnd();

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;

    bool OnMouseWheel(const Point& position, const MouseWheelMessage& message) override;

    /**
     This method is called when the vertical scroll bar is changed.

     @param previous_scroll_bar
         The previous vertical scroll bar.

     Derived classes must call the same method of base class.
     */
    virtual void OnVerticalScrollBarChanged(
        const std::shared_ptr<ScrollBar>& previous_scroll_bar) { }

    /**
     This method is called when the horizontal scroll bar is changed.

     @param previous_scroll_bar
         The previous horizontal scroll bar.

     Derived classes must call the same method of base class.
     */
    virtual void OnHorizontalScrollBarChanged(
        const std::shared_ptr<ScrollBar>& previous_scroll_bar) { }

    /**
     This method is called when the scroll bar corner control is changed.

     @param previous_control
         The previous scroll bar corner control.

     Derived classes must call the same method of base class.
     */
    virtual void OnScrollBarCornerChanged(const std::shared_ptr<Control>& previous_control) { }

    /**
     This method is called when the scroll content control is changed.

     @param previous_control
         The previous scroll content control.

     Derived classes must call the same method of base class.
     */
    virtual void OnScrollContentChanged(const std::shared_ptr<Control>& previous_control) { }

    void OnIsEnabledChanged() override;

private:
    friend class internal::GeneralScrollingLayouter;
    friend class internal::ScrollableControlLayouter;
    friend class internal::SelfScrollingLayouter;

    const std::shared_ptr<Control>& GetScrollContainerControl() const {
        return scroll_container_control_;
    }

    SelfScrollingControl* GetSelfScrollingControl() const {
        return self_scrolling_control_;
    }

private:
    void InitializeScrollContentControl(const std::shared_ptr<Control>& control);
    void InitializeVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeScrollBarCorner(const std::shared_ptr<Control>& corner);
    void InitializeLayouter();
    void OnScrollContentRectChange(const ControlRectChangeInfo& event_info);

private:
    std::shared_ptr<ScrollBar> vertical_scroll_bar_;
    std::shared_ptr<ScrollBar> horizontal_scroll_bar_;
    std::shared_ptr<Control> scroll_bar_corner_;
    std::shared_ptr<Control> scroll_container_control_;
    std::shared_ptr<Control> scroll_content_control_;
    SelfScrollingControl* self_scrolling_control_;

    std::unique_ptr<internal::ScrollableControlLayouter> layouter_;
    bool is_layouting_{};
    Subscription scroll_content_rect_change_subscription_;
};

}