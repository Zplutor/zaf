#pragma once

#include <memory>

namespace zaf {

class Canvas;
class Control;
class Rect;

class Painter {
public:
	virtual ~Painter() { }

	virtual void Paint(Canvas& canvas, const Rect& dirty_rect, const std::shared_ptr<const Control>& control);
};

}