#pragma once

#include <zaf/control/control.h>

namespace zaf {
namespace internal {
class GeneralLayouter;
class ScrollableControlLayouter;
class SelfScrollingLayouter;
}

class ScrollBar;
class SelfScrollingControl;
class Size;

/**
 Represents a scrollable control.

 This is the base class of all scrollable control. 
 
 The scrollable control contains a scroll content control, which would be scrolled when the scroll
 bars changes. You can obatin this control by calling GetScrollContentControl method, and then add
 children to it to build the scroll page. You can also set a custom scroll content control by calling
 SetScrollContentControl method.

 The size of scroll content control is set by SetScrollContentSize method. If the size is larger than
 its scrollable control's visible area size, scroll bars are enabled; otherwise scroll bars are disabled.

 The scroll content control can inherits SelfScrollingControl class. In this way, its scrollable control
 doesn't scroll it anymore, instead, the scroll content control scrolls its own content and controls 
 the scroll bars. For more information, see also SelfScrollingControl.
 */
class ScrollableControl : public Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE

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

    bool AutoAdjustScrollBarLargeChange() const;
    void SetAutoAdjustScrollBarLargeChange(bool value);

    /**
     Get the vertical scroll bar.
     */
    const std::shared_ptr<ScrollBar>& GetVerticalScrollBar() const {
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
    const std::shared_ptr<ScrollBar>& GetHorizontalScrollBar() const {
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
    const std::shared_ptr<Control>& GetScrollBarCorner() const {
        return scroll_bar_corner_;
    }

    /**
     Set the scroll bar corner control.
     */
    void SetScrollBarCorner(const std::shared_ptr<Control>& control);

    /**
     Get the scroll content control.
     */
	const std::shared_ptr<Control>& GetScrollContentControl() const {
        return scroll_content_control_;
	}

    /**
     Set the scroll content control.

     Pass nullptr would set back to the default scroll content control.
     */
    void SetScrollContentControl(const std::shared_ptr<Control>& control);

    /**
     Get scroll bar thickness.

     The default value is 17.
     */
    float GetScrollBarThickness() const;

    /**
     Set scroll bar thickness.
     */
    void SetScrollBarThickness(float thickness);

    /**
     Get scroll content size.
     */
    const Size& GetScrollContentSize() const {
        return scroll_content_control_->GetSize();
    }

    /**
     Set scroll content size.

     The actual scroll content size would be adjusted to the minimum size if 
     the size is not large enough.
     */
    void SetScrollContentSize(const Size& size);

    /**
     Get the visible scroll content rect, in scroll content control's coordinate.
     */
    Rect GetVisibleScrollContentRect() const;

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
	void Layout(const Rect& previous_rect) override;

    bool OnMouseWheel(const Point& position, const MouseWheelMessage& message) override;

    /**
     This method is called when the vertical scroll bar is changed.

     @param previous_scroll_bar
         The previous vertical scroll bar.

     Derived classes must call the same method of base class.
     */
    virtual void VerticalScrollBarChange(const std::shared_ptr<ScrollBar>& previous_scroll_bar) { }

    /**
     This method is called when the horizontal scroll bar is changed.

     @param previous_scroll_bar
         The previous horizontal scroll bar.

     Derived classes must call the same method of base class.
     */
    virtual void HorizontalScrollBarChange(const std::shared_ptr<ScrollBar>& previous_scroll_bar) { }

    /**
     This method is called when the scroll bar corner control is changed.

     @param previous_control
         The previous scroll bar corner control.

     Derived classes must call the same method of base class.
     */
    virtual void ScrollBarCornerChange(const std::shared_ptr<Control>& previous_control) { }

    /**
     This method is called when the scroll content control is changed.

     @param previous_control
         The previous scroll content control.

     Derived classes must call the same method of base class.
     */
    virtual void ScrollContentControlChange(const std::shared_ptr<Control>& previous_control) { }

    void OnIsEnabledChanged() override;

private:
    friend class internal::GeneralLayouter;
    friend class internal::ScrollableControlLayouter;
    friend class internal::SelfScrollingLayouter;

    const std::shared_ptr<Control>& GetScrollContainerControl() const {
        return scroll_container_control_;
    }

    const Size& GetExpectedScrollContentSize() const {
        return expected_scroll_content_size_;
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

private:
    std::shared_ptr<ScrollBar> vertical_scroll_bar_;
    std::shared_ptr<ScrollBar> horizontal_scroll_bar_;
    std::shared_ptr<Control> scroll_bar_corner_;
    std::shared_ptr<Control> scroll_container_control_;
	std::shared_ptr<Control> scroll_content_control_;
    SelfScrollingControl* self_scrolling_control_;

	Size expected_scroll_content_size_;
    std::unique_ptr<internal::ScrollableControlLayouter> layouter_;
};

}