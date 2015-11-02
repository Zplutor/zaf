#pragma once

#include <memory>

namespace zaf {
namespace internal {
namespace theme {

class Painter;

class Theme {
public:
	const std::shared_ptr<Painter> GetDefaultPainter() {
		if (default_painter_ == nullptr) {
			default_painter_ = CreateDefaultPainter();
		}
		return default_painter_;
	}

	const std::shared_ptr<Painter> GetButtonPainter() {
		if (button_painter_ == nullptr) {
			button_painter_ = CreateButtonPainter();
		}
		return button_painter_;
	}

	const std::shared_ptr<Painter> GetLabelPainter() {
		if (label_painter_ == nullptr) {
			label_painter_ = CreateLabelPainter();
		}
		return label_painter_;
	}

	const std::shared_ptr<Painter> GetScrollBarPainter() {
		if (scroll_bar_painter_ == nullptr) {
			scroll_bar_painter_ = CreateScrollBarPainter();
		}
		return scroll_bar_painter_;
	}

	const std::shared_ptr<Painter> GetScrollBarArrowPainter() {
		if (scroll_bar_arrow_painter_ == nullptr) {
			scroll_bar_arrow_painter_ = CreateScrollBarArrowPainter();
		}
		return scroll_bar_arrow_painter_;
	}

	const std::shared_ptr<Painter> GetScrollBarThumbPainter() {
		if (scroll_bar_thumb_painter_ == nullptr) {
			scroll_bar_thumb_painter_ = CreateScrollBarThumbPainter();
		}
		return scroll_bar_thumb_painter_;
	}

	const std::shared_ptr<Painter> GetScrollableControlPainter() {
		if (scrollable_control_painter_ == nullptr) {
			scrollable_control_painter_ = CreateScrollableControlPainter();
		}
		return scrollable_control_painter_;
	}

protected:
	virtual std::shared_ptr<Painter> CreateDefaultPainter() = 0;
	virtual std::shared_ptr<Painter> CreateButtonPainter() = 0;
	virtual std::shared_ptr<Painter> CreateLabelPainter() = 0;
	virtual std::shared_ptr<Painter> CreateScrollBarPainter() = 0;
	virtual std::shared_ptr<Painter> CreateScrollBarArrowPainter() = 0;
	virtual std::shared_ptr<Painter> CreateScrollBarThumbPainter() = 0;
	virtual std::shared_ptr<Painter> CreateScrollableControlPainter() = 0;

private:
	std::shared_ptr<Painter> default_painter_;
	std::shared_ptr<Painter> button_painter_;
	std::shared_ptr<Painter> label_painter_;
	std::shared_ptr<Painter> scroll_bar_painter_;
	std::shared_ptr<Painter> scroll_bar_arrow_painter_;
	std::shared_ptr<Painter> scroll_bar_thumb_painter_;
	std::shared_ptr<Painter> scrollable_control_painter_;
};


std::shared_ptr<Theme> GetTheme();
void SetTheme(const std::shared_ptr<Theme>& theme);

}
}
}