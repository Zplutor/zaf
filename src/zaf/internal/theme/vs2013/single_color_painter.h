#pragma once

#include <zaf/graphic/color.h>
#include <zaf/internal/theme/painter.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

class SingleColorPainter : public Painter {
public:
	SingleColorPainter(const Color& color) : color_(color) { }

	void Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) override;

private:
	Color color_;
};

}
}
}
}