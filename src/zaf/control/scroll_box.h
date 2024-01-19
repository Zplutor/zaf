#pragma once

#include <zaf/control/control.h>
#include <zaf/rx/subscription.h>

namespace zaf {
namespace internal {
class GeneralScrollLayouter;
class ScrollBoxLayouter;
class SelfScrollLayouter;
}

class ScrollBar;
class SelfScrollControl;
class zaf::Size;

/**
Represents a container that can scroll its content.

A ScrollBox is designed to scroll its scroll content rather than its child controls. The scroll
content is a single control which is set by SetScrollContent(). A control becomes scrollable after 
being set as the scroll content of a ScrollBox.

By default, the scroll content will be resized to match the ScrollBox's visible area automatically,
thus preventing the scroll bars from being enabled. To enable the scroll bars, you can either set 
a fixed size to the scroll content to prevent auto-sizing, or call SetAutoScrollContentSize() with
a false value to disable auto-sizing.

The scroll content control can inherit SelfScrollControl. In this way, the scroll content becomes 
responsible for scrolling the content by implementing the methods of SelfScrollControl. For more 
information, see also SelfScrollControl.
*/
class ScrollBox : public Control {
public:
    ZAF_DECLARE_TYPE

public:
    ScrollBox();
    ~ScrollBox();

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

    /**
    Gets a value indicating whether the scroll bars overlay the content.

    The default value is false.
    */
    bool UseOverlayScrollBars() const;

    /**
    Sets whether the scroll bars overlay the content.
    */
    void SetUseOverlayScrollBars(bool use);

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
     Get vertical scroll bar thickness.

     The default value is 17.
     */
    float VerticalScrollBarThickness() const;

    /**
     Set vertical scroll bar thickness.
     */
    void SetVerticalScrollBarThickness(float thickness);

    /**
     Get horizontal scroll bar thickness.

     The default value is 17.
     */
    float HorizontalScrollBarThickness() const;

    /**
     Set horizontal scroll bar thickness.
     */
    void SetHorizontalScrollBarThickness(float thickness);

    /**
    Set both vertical and horizontal scroll bar thickness.
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

    /**
    Scrolls to the top of the scroll content.
    */
    void ScrollToTop();

    /**
    Scrolls to the bottom of the scroll content.
    */
    void ScrollToBottom();

    /**
    Scrolls to the left of the scroll content.
    */
    void ScrollToLeft();

    /**
    Scrolls to the right of the scroll content.
    */
    void ScrollToRight();

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;

    void OnMouseWheel(const MouseWheelInfo& event_info) override;

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

private:
    friend class internal::GeneralScrollLayouter;
    friend class internal::ScrollBoxLayouter;
    friend class internal::SelfScrollLayouter;

    const std::shared_ptr<Control>& GetScrollContainerControl() const {
        return scroll_container_control_;
    }

    SelfScrollControl* GetSelfScrollingControl() const {
        return self_scrolling_control_;
    }

private:
    void InitializeScrollContentControl(const std::shared_ptr<Control>& control);
    void InitializeVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeScrollBarCorner(const std::shared_ptr<Control>& corner);
    void InitializeLayouter();
    void OnScrollContentRectChange(const RectChangedInfo& event_info);

private:
    std::shared_ptr<ScrollBar> vertical_scroll_bar_;
    std::shared_ptr<ScrollBar> horizontal_scroll_bar_;
    std::shared_ptr<Control> scroll_bar_corner_;
    std::shared_ptr<Control> scroll_container_control_;
    std::shared_ptr<Control> scroll_content_control_;
    SelfScrollControl* self_scrolling_control_;

    bool allow_vertical_scroll_{ true };
    bool allow_horizontal_scroll_{ true };
    bool auto_scroll_bar_large_change_{ true };
    bool auto_scroll_content_width_{ true };
    bool auto_scroll_content_height_{ true };
    bool auto_hide_scroll_bars_{ false };
    bool use_overlay_scroll_bars_{ false };
    float vertical_scroll_bar_thickness_{ 17 };
    float horizontal_scroll_bar_thickness_{ 17 };

    std::unique_ptr<internal::ScrollBoxLayouter> layouter_;
    Subscription scroll_content_rect_change_subscription_;
};

}