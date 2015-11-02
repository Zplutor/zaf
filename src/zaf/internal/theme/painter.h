#pragma once

namespace zaf {

class Canvas;
class Control;
class Rect;

namespace internal {
namespace theme {

class Painter {
public:
	virtual void Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) = 0;
};

}
}
}