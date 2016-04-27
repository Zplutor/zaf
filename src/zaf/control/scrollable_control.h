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
    void SetAutoHideScrollBars(bool always_show);

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

    void MouseWheel(const Point& position, bool is_horizontal, int distance, WPARAM wParam, LPARAM lParam) override;

	void ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar);

private:
    void InitializeVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
    void InitializeScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);

    void InitializeScrolledControl(const std::shared_ptr<Control>& control);

    void CanShowScrollBars(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const;
    void CanShowScrollBarsWithGeneralScrolledControl(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const;
    void CanShowScrollBarsWithSelScrollingControl(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const;

    void LayoutScrollBars(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);
    void LayoutScrollContainerControl(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);

    void AdjustScrolledControlSize(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);
    void AdjustGeneralScrolledControlSize(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);
    void AdjustSelfScrollingControlSize();

	void AdjustScrollBarValues();
    void AdjustScrollBarValuesWithGeneralScrolledControl();
    void AdjustScrollBarValuesWithSelfScrollingControl();

private:
    std::shared_ptr<ScrollBar> vertical_scroll_bar_;
    std::shared_ptr<ScrollBar> horizontal_scroll_bar_;
    std::shared_ptr<Control> scroll_container_control_;
	std::shared_ptr<Control> scrolled_control_;
    SelfScrollingControl* self_scrolling_control_;

	Size expected_scroll_area_size_;
};

}