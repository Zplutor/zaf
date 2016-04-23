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

	void LayoutScrollBars();
	void LayoutScrollAreaControl();

	void AdjustScrolledControlSize();
    void AdjustGeneralScrolledControlSize();
    void AdjustSelfScrollingControlSize();

	void AdjustScrollBarValues();
    void AdjustScrollBarValuesWithGeneralScrolledControl();
    void AdjustScrollBarValuesWithSelfScrollingControl();

private:
    std::shared_ptr<ScrollBar> vertical_scroll_bar_;
    std::shared_ptr<ScrollBar> horizontal_scroll_bar_;
    std::shared_ptr<Control> scroll_area_control_;
	std::shared_ptr<Control> scrolled_control_;
    SelfScrollingControl* self_scrolling_control_;

	Size expected_scroll_area_size_;
};

}