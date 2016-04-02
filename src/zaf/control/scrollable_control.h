#pragma once

#include <zaf/control/control.h>

namespace zaf {

class ScrollBar;
class Size;

class ScrollableControl : public Control {
public:
	ScrollableControl();
	~ScrollableControl();

	void Initialize() override;

	void SetVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);
	void SetHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar);

	const std::shared_ptr<Control>& GetContentControl() const {
		return content_control_;
	}

	const Size& GetContentSize() const {
		return content_control_->GetSize();
	}

	void SetContentSize(const Size& size);

protected:
	void Layout(const Rect& previous_rect) override;

    void MouseWheel(const Point& position, bool is_horizontal, int distance, WPARAM wParam, LPARAM lParam) override;

	void ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar);

private:
	void LayoutScrollBars();
	void LayoutContentContainerControl();
	void AdjustContentControlSize();
	void AdjustScrollBarValues();

private:
	std::shared_ptr<Control> content_control_;
	std::shared_ptr<Control> content_container_control_;
	std::shared_ptr<ScrollBar> vertical_scroll_bar_;
	std::shared_ptr<ScrollBar> horizontal_scroll_bar_;

	Size content_size_;
};

}