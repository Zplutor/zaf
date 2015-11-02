#pragma once

#include <zaf/internal/theme/painter.h>

namespace zaf {

class Button;
class Color;

namespace internal {
namespace theme {
namespace vs2013 {

class ButtonPainter : public Painter {
public:
	void Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) override;
	
private:
	const Color& GetBackgroundColor(const Button& button);
	const Color& GetBorderColor(const Button& button);
};

}
}
}
}