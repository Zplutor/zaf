#pragma once

#include <zaf/internal/theme/theme.h>
#include <zaf/internal/theme/vs2013/button_painter.h>
#include <zaf/internal/theme/vs2013/label_painter.h>
#include <zaf/internal/theme/vs2013/palette.h>
#include <zaf/internal/theme/vs2013/scroll_bar_painter.h>
#include <zaf/internal/theme/vs2013/single_color_painter.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

class Theme : public theme::Theme {
protected:
	std::shared_ptr<Painter> CreateDefaultPainter() override {
		return std::make_shared<SingleColorPainter>(kControlBackgroundColor);
	}

	std::shared_ptr<Painter> CreateButtonPainter() override {
		return std::make_shared<ButtonPainter>();
	}

	std::shared_ptr<Painter> CreateLabelPainter() override {
		return std::make_shared<LabelPainter>();
	}

	std::shared_ptr<Painter> CreateScrollBarPainter() override {
		return std::make_shared<ScrollBarPainter>();
	}

	std::shared_ptr<Painter> CreateScrollBarArrowPainter() override {
		return std::make_shared<ScrollBarArrowPainter>();
	}

	std::shared_ptr<Painter> CreateScrollBarThumbPainter() override {
		return std::make_shared<ScrollBarThumbPainter>();
	}

	std::shared_ptr<Painter> CreateScrollableControlPainter() override {
		return std::make_shared<SingleColorPainter>(kScrollBarBackgroundColor);
	}
};

}
}
}
}
