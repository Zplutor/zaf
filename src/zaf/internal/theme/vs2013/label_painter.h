#pragma once

#include <zaf/internal/theme/painter.h>

namespace zaf {

class Color;

namespace internal {
namespace theme {
namespace vs2013 {

class LabelPainter : public Painter {
public:
	void Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) override;
};

}
}
}
}