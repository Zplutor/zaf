#pragma once

#include <zaf/control/control.h>

namespace zaf {

class ScrollBar;
class SelfScrollingControl;
class Size;

class ScrollableControl : public Control {
public:
	ScrollableControl();
	~ScrollableControl();

	void Initialize() override;

    bool AllowVerticalScroll() const;
    void SetAllowVerticalScroll(bool allow_scroll);

    bool AllowHorizontalScroll() const;
    void SetAllowHorizontalScroll(bool allow_scroll);

    bool AutoHideScrollBars() const;
    void SetAutoHideScrollBars(bool auto_hide);

    const std::shared_ptr<ScrollBar>& GetVerticalScrollBar() const {
        return vertical_scroll_bar_;
    }

    void SetVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);

    const std::shared_ptr<ScrollBar>& GetHorizontalScrollBar() const {
        return horizontal_scroll_bar_;
    }

    void SetHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);

	const std::shared_ptr<Control>& GetScrolledControl() const {
        return scrolled_control_;
	}

    void SetScrolledControl(const std::shared_ptr<Control>& control);

    float GetScrollBarThickness() const;
    void SetScrollBarThickness(float thickness);

    const Size& GetScrollAreaSize() const {
        return scrolled_control_->GetSize();
    }

    void SetScrollAreaSize(const Size& size);

protected:
	void Layout(const Rect& previous_rect) override;

    void MouseWheel(const Point& position, const MouseWheelMessage& message) override;

    const Rect GetVisibleScrollAreaRect() const;
    void ScrollToScrollAreaPosition(const Point& position);

private:
    void InitializeVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);

    void InitializeScrolledControl(const std::shared_ptr<Control>& control);

    void LayoutWithGeneralScrolledControl();
    void CanShowScrollBars(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const;

    void LayoutScrollBars(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);
    void LayoutScrollContainerControl(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);
    void LayoutScrolledControlSize(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);

    void AdjustScrollBarValuesWithGeneralScrolledControl();

    void LayoutWithSelfScrollingControl();
    void AdjustScrollBarValueWithSelfScrollingControl(bool is_horizontal);
    
    void ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar);
    void ScrollGeneralScrolledControl(const std::shared_ptr<ScrollBar>& scroll_bar);
    void ScrollSelfScrollingControl(const std::shared_ptr<ScrollBar>& scroll_bar);

    void SelfScrollingControlScrollBarChange(SelfScrollingControl& self_scrolling_control);
    void SelfScrollingControlScrollValuesChange(SelfScrollingControl& self_scrolling_control, bool is_horizontal);

private:
    std::shared_ptr<ScrollBar> vertical_scroll_bar_;
    std::shared_ptr<ScrollBar> horizontal_scroll_bar_;
    std::shared_ptr<Control> scroll_container_control_;
	std::shared_ptr<Control> scrolled_control_;

    SelfScrollingControl* self_scrolling_control_;
    bool is_self_scrolling_;

	Size expected_scroll_area_size_;
};

}