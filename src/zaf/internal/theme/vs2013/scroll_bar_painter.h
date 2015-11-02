#pragma once

#include <zaf/internal/theme/painter.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

class ScrollBarPainter : public Painter {
public:
	void Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) override;
};

class ScrollBarArrowPainter : public Painter {
public:
	void Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) override;
};


class ScrollBarThumbPainter : public Painter {
public:
	void Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) override;
};

}
}
}
}