#pragma once

#include <zaf/internal/theme/painter.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

class CheckBoxPainter : public Painter {
public:
	void Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) override;
};

}
}
}
}