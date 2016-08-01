#pragma once

#include <zaf/control/control.h>

namespace zaf {

class ScrollBar;
class SelfScrollingControl;
class Size;

/**
 Represents a scrollable control.

 This is the base class of all scrollable control.
 */
class ScrollableControl : public Control {
public:
	ScrollableControl();
	~ScrollableControl();

	void Initialize() override;

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

protected:
	void Layout(const Rect& previous_rect) override;

    void MouseWheel(const Point& position, const MouseWheelMessage& message) override;

    /**
     Get the visible scroll content rect, in scroll content control's coordinate.
     */
    const Rect GetVisibleScrollContentRect() const;

    /**
     Scroll the scroll content to specified position, in scroll content control's coordinate.

     The specified position occurs at left-top cornor of the control after scrolling.
     */
    void ScrollToScrollContentPosition(const Point& position);

private:
    void InitializeVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);

    void InitializeScrollContentControl(const std::shared_ptr<Control>& control);

    void LayoutWithGeneralScrollContentControl();
    void CanShowScrollBars(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const;

    void LayoutScrollBars(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);
    void LayoutScrollContainerControl(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);
    void LayoutScrollContentControlSize(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);

    void AdjustScrollBarValuesWithGeneralScrollContentControl();

    void LayoutWithSelfScrollingControl();
    void AdjustScrollBarValueWithSelfScrollingControl(bool is_horizontal);
    
    void ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar);
    void ScrollGeneralScrollContentControl(const std::shared_ptr<ScrollBar>& scroll_bar);
    void ScrollSelfScrollingControl(const std::shared_ptr<ScrollBar>& scroll_bar);

    void SelfScrollingControlScrollBarChange(SelfScrollingControl& self_scrolling_control);
    void SelfScrollingControlScrollValuesChange(SelfScrollingControl& self_scrolling_control, bool is_horizontal);

private:
    std::shared_ptr<ScrollBar> vertical_scroll_bar_;
    std::shared_ptr<ScrollBar> horizontal_scroll_bar_;
    std::shared_ptr<Control> scroll_container_control_;
	std::shared_ptr<Control> scroll_content_control_;

    SelfScrollingControl* self_scrolling_control_;
    bool is_self_scrolling_;

	Size expected_scroll_content_size_;
};

}